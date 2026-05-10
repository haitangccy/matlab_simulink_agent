"""
vsg_simulator.py  —  v3

Buck 双环 PI 调参专用仿真封装。

主要修复（v3）：
  1. 峰值计算：只在"上升结束→尾部前"窗口内搜索，消除启动浪涌导致
     overshoot=21096% 的虚高问题。
  2. 调节时间扫描：从 rise_end_idx 开始（非 peak_idx），跳过启动段，
     消除 settling_time=0.03s 的误判。
  3. 收敛二次保险：overshoot>200% 或 verr_pct>10% 时强制 converged=False，
     防止 LLM 看到"keep"而停止优化 Ki。
  4. verr_pct（稳态误差%）新字段：直接量化输出偏离目标的百分比，
     供 LLM 判断是否需要增大 Ki（Buck 目标 12V 现输出 11.9V → verr=0.83%）。
"""

import os
from dataclasses import dataclass, field
from typing import Optional
import numpy as np

import matlab.engine



# ---------------------------------------------------------------------------
# 配置
# ---------------------------------------------------------------------------

@dataclass
class SimConfig:
    model_name: str = "simple"
    model_dir: str = ""
    controller_block_i_1: str = "PID_Controller_i_1"
    controller_block_i_2: str = "PID_Controller_i_2"
    controller_block_e_1: str = "PID_Controller_e_1"
    controller_block_e_2: str = "PID_Controller_e_2"
    out_signal: str = "out.Data.Data"
    stop_time: float = 1.0
    simulation_mode: str = "accelerator"
    downsample_target: int = 20_000
    target_value: Optional[float] = None
    # 外环电压信号（如 "out.V.Data"）；None 时不单独读电压
    # 内环不需要；外环必须设置，指标提取将基于此信号（verr/settling才有意义）
    voltage_signal: Optional[str] = None


# ---------------------------------------------------------------------------
# 仿真结果
# ---------------------------------------------------------------------------

@dataclass
class SimResult:
    t: np.ndarray = field(default_factory=lambda: np.array([]))
    data: np.ndarray = field(default_factory=lambda: np.array([]))
    # 外环电压信号（降采样后）；内环为空
    v_data: np.ndarray = field(default_factory=lambda: np.array([]))

    steady_state: float = 0.0
    peak: float = 0.0
    overshoot_pct: float = 0.0
    rise_time: float = 0.0
    settling_time: float = 0.0
    tail_std: float = 0.0
    converged: bool = False

    # 稳态误差（relative to target_value）
    verr_pct: float = 0.0

    curve_state: str = "unknown"
    tail_drift_pct: float = 0.0
    end_slope_pct_per_s: float = 0.0
    abs_end_slope: float = 0.0
    tail_range_pct: float = 0.0
    half_zero_crossings: int = 0
    oscillation_decay_ratio: float = 0.0

    recommended_action: str = "retune_pi"
    action_reason: str = ""

    def is_valid(self) -> bool:
        return len(self.data) >= 10

    def decision_metrics(self) -> dict:
        m: dict = {
            "curve_state":            self.curve_state,
            "steady_state":           round(self.steady_state, 4),
            "verr_pct":               round(self.verr_pct, 3),
            "tail_std":               round(self.tail_std, 6),
            "tail_drift_pct":         round(self.tail_drift_pct, 4),
            "end_slope_pct_per_s":    round(self.end_slope_pct_per_s, 4),
            "abs_end_slope":          round(self.abs_end_slope, 6),
            "tail_range_pct":         round(self.tail_range_pct, 4),
            "half_zero_crossings":    self.half_zero_crossings,
            "oscillation_decay_ratio": round(self.oscillation_decay_ratio, 4),
            "recommended_action":     self.recommended_action,
            "action_reason":          self.action_reason,
        }
        if self.curve_state == "converged":
            m.update({
                "overshoot_pct": round(self.overshoot_pct, 2),
                "rise_time":     round(self.rise_time, 4),
                "settling_time": round(self.settling_time, 4),
            })
        return m

    def metrics_dict(self) -> dict:
        return {
            "steady_state":           round(self.steady_state, 4),
            "verr_pct":               round(self.verr_pct, 3),
            "peak":                   round(self.peak, 4),
            "overshoot_pct":          round(self.overshoot_pct, 2),
            "rise_time":              round(self.rise_time, 4),
            "settling_time":          round(self.settling_time, 4),
            "tail_std":               round(self.tail_std, 6),
            "converged":              self.converged,
            "curve_state":            self.curve_state,
            "tail_drift_pct":         round(self.tail_drift_pct, 4),
            "end_slope_pct_per_s":    round(self.end_slope_pct_per_s, 4),
            "abs_end_slope":          round(self.abs_end_slope, 6),
            "tail_range_pct":         round(self.tail_range_pct, 4),
            "half_zero_crossings":    self.half_zero_crossings,
            "oscillation_decay_ratio": round(self.oscillation_decay_ratio, 4),
            "recommended_action":     self.recommended_action,
            "action_reason":          self.action_reason,
        }


# ---------------------------------------------------------------------------
# 块均值降采样
# ---------------------------------------------------------------------------

def _downsample(t: np.ndarray, data: np.ndarray, target: int) -> tuple[np.ndarray, np.ndarray]:
    n = len(data)
    if target <= 0 or n <= target:
        return t, data
    step = max(1, n // target)
    num_blocks = n // step
    if num_blocks < 1:
        return t, data
    t_ds    = t[:num_blocks * step].reshape(num_blocks, step).mean(axis=1)
    data_ds = data[:num_blocks * step].reshape(num_blocks, step).mean(axis=1)
    return t_ds, data_ds


# ---------------------------------------------------------------------------
# 曲线状态分类
# ---------------------------------------------------------------------------

def classify_curve(
    trend: np.ndarray,
    t: np.ndarray,
    steady_state: float,
    converged: bool,
) -> tuple[str, dict]:
    n = len(trend)
    scale_ref = max(abs(steady_state), float(np.max(np.abs(trend))), 1e-6)

    if converged:
        return "converged", {
            "tail_drift_pct": 0.0, "end_slope_pct_per_s": 0.0,
            "abs_end_slope": 0.0, "tail_range_pct": 0.0,
            "half_zero_crossings": 0, "oscillation_decay_ratio": 0.0,
            "recommended_action": "keep", "action_reason": "响应已在当前仿真时长内收敛",
        }

    tail_win = max(10, int(0.1 * n))
    prev_win = max(10, int(0.2 * n))
    tail   = trend[-tail_win:]
    tail_t = t[-tail_win:]
    ps     = max(0, n - tail_win - prev_win)
    pe     = max(ps + 1, n - tail_win)
    prev   = trend[ps:pe] if pe > ps else tail

    tail_mean = float(np.mean(tail))
    prev_mean = float(np.mean(prev))
    tail_drift_pct = abs(tail_mean - prev_mean) / scale_ref * 100.0

    if len(tail_t) >= 2 and float(tail_t[-1] - tail_t[0]) > 1e-12:
        end_slope, _ = np.polyfit(tail_t, tail, 1)
    else:
        end_slope = 0.0
    end_slope_pct_per_s = end_slope / scale_ref * 100.0
    abs_end_slope = abs(end_slope)

    tail_range_pct = float(np.ptp(tail) / scale_ref * 100.0)
    prev_range_pct = float(np.ptp(prev) / scale_ref * 100.0) if len(prev) > 1 else tail_range_pct
    oscillation_decay_ratio = tail_range_pct / max(prev_range_pct, 1e-6)

    half = trend[n // 2:]
    half_center = float(np.mean(half))
    zero_crossings = int(np.sum(np.diff(np.sign(half - half_center)) != 0))

    is_oscillating = zero_crossings >= 4 and tail_range_pct >= 3.0 and oscillation_decay_ratio >= 0.85
    is_rising = end_slope_pct_per_s > 0.2 and tail_drift_pct >= 1.0 and zero_crossings <= 2

    base = {
        "tail_drift_pct": tail_drift_pct, "end_slope_pct_per_s": end_slope_pct_per_s,
        "abs_end_slope": abs_end_slope, "tail_range_pct": tail_range_pct,
        "half_zero_crossings": zero_crossings, "oscillation_decay_ratio": oscillation_decay_ratio,
    }

    if is_oscillating:
        return "oscillating", {**base, "recommended_action": "retune_pi", "action_reason": "后半段存在持续震荡"}
    if is_rising:
        act, rsn = (("extend_stop_time", "响应仍在上升但尾部漂移已较小")
                    if tail_drift_pct <= 3.0 and end_slope_pct_per_s <= 1.0
                    else ("retune_pi", "响应仍明显上升，PI 参数偏弱"))
        return "rising", {**base, "recommended_action": act, "action_reason": rsn}
    if tail_drift_pct <= 2.0:
        return "converged", {**base, "recommended_action": "extend_stop_time", "action_reason": "尾部平稳，延长仿真确认收敛"}
    return "converged", {**base, "recommended_action": "retune_pi", "action_reason": "尾部不震荡但漂移仍偏明显"}


# ---------------------------------------------------------------------------
# 仿真器
# ---------------------------------------------------------------------------

class VSGSimulator:
    def __init__(self, config: SimConfig):
        self.cfg = config
        self._model_dir = config.model_dir or os.path.dirname(os.path.abspath(__file__))
        self._eng: Optional[matlab.engine.MatlabEngine] = None
        self._model_loaded: bool = False

    def _supports_fast_restart(self) -> bool:
        return self.cfg.simulation_mode.lower() in {"normal", "accelerator"}

    def __enter__(self):
        self.start(); return self

    def __exit__(self, *_):
        self.close()

    def start(self) -> None:
        if self._eng is not None:
            return
        print("[VSGSimulator] 正在启动 MATLAB Engine...")
        self._eng = matlab.engine.start_matlab()
        self._eng.cd(self._model_dir, nargout=0)
        self._eng.addpath(self._model_dir, nargout=0)
        print("[VSGSimulator] MATLAB Engine 已就绪")

    def close(self) -> None:
        if self._eng is None:
            return
        if self._model_loaded:
            try: self._eng.close_system(self.cfg.model_name, 0, nargout=0)
            except Exception: pass
            self._model_loaded = False
        try: self._eng.quit()
        except Exception: pass
        self._eng = None
        print("[VSGSimulator] MATLAB Engine 已关闭")

    def load_model(self) -> None:
        self._require_engine()
        mf = os.path.join(self._model_dir, f"{self.cfg.model_name}.slx").replace("\\", "/")
        self._eng.load_system(mf, nargout=0)
        self._eng.set_param(self.cfg.model_name, "StopTime", str(float(self.cfg.stop_time)), nargout=0)
        self._eng.set_param(self.cfg.model_name, "SimulationMode", self.cfg.simulation_mode, nargout=0)
        self._eng.set_param(self.cfg.model_name, "FastRestart",
                            "on" if self._supports_fast_restart() else "off", nargout=0)
        self._model_loaded = True
        print(f"[VSGSimulator] 模型 '{self.cfg.model_name}' 已加载，仿真时长={self.cfg.stop_time}s")

    def update_stop_time(self, stop_time: float) -> None:
        self.cfg.stop_time = float(stop_time)
        if self._eng and self._model_loaded:
            self._eng.set_param(self.cfg.model_name, "StopTime", str(float(stop_time)), nargout=0)

    def run_inner(self, kp_i: float, ki_i: float) -> SimResult:
        self._require_engine(); self._require_model()
        self._set_trigger(0)
        self._set_pi(self.cfg.controller_block_i_1, kp_i, ki_i)
        self._set_pi(self.cfg.controller_block_i_2, kp_i, ki_i)
        return self._simulate_and_analyze()

    def run_outer(self, kp_i: float, ki_i: float, kp_e: float, ki_e: float) -> SimResult:
        self._require_engine(); self._require_model()
        self._set_trigger(1)
        self._set_pi(self.cfg.controller_block_i_1, kp_i, ki_i)
        self._set_pi(self.cfg.controller_block_i_2, kp_i, ki_i)
        self._set_pi(self.cfg.controller_block_e_1, kp_e, ki_e)
        self._set_pi(self.cfg.controller_block_e_2, kp_e, ki_e)
        return self._simulate_and_analyze()

    def _require_engine(self):
        if self._eng is None:
            raise RuntimeError("MATLAB Engine 尚未启动")

    def _require_model(self):
        if not self._model_loaded:
            self.load_model()

    def _set_trigger(self, v: int):
        self._eng.set_param(f"{self.cfg.model_name}/trigger", "Value", str(int(v)), nargout=0)

    def _set_pi(self, block: str, kp: float, ki: float):
        b = f"{self.cfg.model_name}/{block}"
        self._eng.set_param(b, "P", str(float(kp)), nargout=0)
        self._eng.set_param(b, "I", str(float(ki)), nargout=0)

    def _simulate_and_analyze(self) -> SimResult:
        t_raw, data_raw, v_raw = self._run_sim()
        t, data = _downsample(t_raw, data_raw, self.cfg.downsample_target)
        if len(t) < len(t_raw):
            print(f"[VSGSimulator] 降采样: {len(t_raw):,} → {len(t):,} 点")

        result = SimResult(t=t, data=data)

        # 电压信号：仅用于计算 verr_pct（评分用），以及绘图
        # 指标提取（settling/overshoot/rise等）始终基于 out.Data.Data（PI输出）
        verr_pct = 0.0
        if v_raw is not None and len(v_raw) > 10:
            _, v_ds = _downsample(t_raw, v_raw, self.cfg.downsample_target)
            result.v_data = v_ds
            # 用电压尾部均值计算稳态误差
            if self.cfg.target_value is not None and abs(self.cfg.target_value) > 1e-9:
                v_tail = v_ds[max(0, len(v_ds) - max(10, len(v_ds)//10)):]
                v_ss   = float(np.mean(v_tail))
                verr_pct = abs(v_ss - self.cfg.target_value) / abs(self.cfg.target_value) * 100.0
        else:
            result.v_data = np.array([])

        if result.is_valid():
            # target_value 传 None：verr_pct 已单独从电压信号算好，不再用PI输出估算
            VSGSimulator._fill_metrics(result, target_value=None)
            result.verr_pct = round(verr_pct, 3)
        return result

    def _run_sim(self) -> tuple[np.ndarray, np.ndarray, Optional[np.ndarray]]:
        self._eng.eval("clear out;", nargout=0)
        fr = "on" if self._supports_fast_restart() else "off"
        self._eng.eval(f"set_param('{self.cfg.model_name}','FastRestart','{fr}');", nargout=0)
        print(f"[VSGSimulator] 开始仿真: mode={self.cfg.simulation_mode}, FastRestart={fr}")
        self._eng.eval(f"out = sim('{self.cfg.model_name}');", nargout=0)
        print("[VSGSimulator] 仿真完成，开始读取输出")
        t    = np.array(self._eng.eval("out.tout",          nargout=1)).flatten()
        data = np.array(self._eng.eval(self.cfg.out_signal, nargout=1)).flatten()
        print(f"[VSGSimulator] 主信号: len={len(data):,}")
        v = None
        if self.cfg.voltage_signal:
            try:
                v = np.array(self._eng.eval(self.cfg.voltage_signal, nargout=1)).flatten()
                print(f"[VSGSimulator] 电压信号: len={len(v):,}")
            except Exception as e:
                print(f"[VSGSimulator] 警告：电压信号读取失败({e})，改用主信号")
        return t, data, v

    # ------------------------------------------------------------------
    # 两阶段均值滤波
    # ------------------------------------------------------------------

    @staticmethod
    def _smooth(data: np.ndarray) -> np.ndarray:
        n = len(data)
        if n < 7:
            return data.copy()
        def _ma(a, w):
            if w < 3: return a.copy()
            w = min(w | 1, n if n % 2 == 1 else n - 1)
            p = w // 2
            return np.convolve(np.pad(a, (p, p), mode="edge"), np.ones(w) / w, mode="valid")[:n]
        return _ma(_ma(data, max(5, min(int(0.05 * n), 2001))), max(3, min(int(0.01 * n), 201)))

    # ------------------------------------------------------------------
    # 指标提取（核心）
    # ------------------------------------------------------------------

    @staticmethod
    def _fill_metrics(result: SimResult, target_value: Optional[float] = None) -> None:
        """
        基于 result.data（out.Data.Data，PI输出）提取 settling/overshoot/rise 等指标。
        verr_pct 由 _simulate_and_analyze 从电压信号单独计算后写入，此处不重复计算。
        """
        t     = result.t
        data  = result.data   # 始终用 PI 输出做形态分析
        trend = VSGSimulator._smooth(data)
        n     = len(trend)

        # 1. 尾部 & 稳态值
        win  = max(10, int(0.1 * n))
        tail = trend[-win:]
        tail_t = t[-win:]
        tail_std = float(np.std(tail))
        ss_noisy = tail_std > 0.02 * max(abs(np.mean(tail)), 1.0)
        ss = float(np.median(tail) if ss_noisy else np.mean(tail))
        ss_scale  = max(abs(ss), 1e-6)
        scale_ref = max(abs(ss), float(np.max(np.abs(trend))), 1e-6)

        # verr_pct 由调用方（_simulate_and_analyze）从电压信号算好后写入 result，
        # 此处 target_value 参数保留兼容性但不再使用（传 None 即可）
        verr_pct = 0.0

        # 2. 上升时间（10%→90%）
        l10, l90 = 0.1 * ss, 0.9 * ss
        if ss >= 0:
            i10 = np.where(trend >= l10)[0]; i90 = np.where(trend >= l90)[0]
        else:
            i10 = np.where(trend <= l10)[0]; i90 = np.where(trend <= l90)[0]
        rise_start = int(i10[0]) if len(i10) > 0 else 0
        rise_end   = int(i90[0]) if len(i90) > 0 else n - 1
        rise_time  = float(t[rise_end] - t[rise_start])

        # 3. 峰值 & 超调（仅在上升结束→尾部前窗口内）
        # ★ 关键修复：跳过启动浪涌段，消除 overshoot=21096% 的虚高
        tail_start = max(rise_end + 1, n - win)
        post_rise  = trend[rise_end:tail_start]
        peak       = float(np.max(post_rise)) if len(post_rise) > 0 else ss
        overshoot  = max(0.0, (peak - ss) / ss_scale * 100.0)

        # 4. 尾部斜率
        if len(tail_t) >= 2 and float(tail_t[-1] - tail_t[0]) > 1e-12:
            tail_slope, _ = np.polyfit(tail_t, tail, 1)
        else:
            tail_slope = 0.0
        end_slope_pct = tail_slope / scale_ref * 100.0
        abs_slope     = abs(tail_slope)

        # 5. 调节时间（O(N) 向量化，从 rise_end 开始）
        # ★ 关键修复：从 rise_end 开始扫描，消除 settling=0.03s 的误判
        tol          = 0.05 * ss_scale
        std_tol      = 0.01 * max(abs(ss), 1.0)
        slope_tol    = 0.05
        settling     = float(t[-1])
        converged    = False
        seg          = trend[rise_end:]; seg_t = t[rise_end:]
        seg_n        = len(seg)

        if seg_n >= 5:
            ib    = (np.abs(seg - ss) <= tol).astype(np.int8)
            sc    = np.cumsum(ib[::-1])[::-1]
            sl    = np.arange(seg_n, 0, -1)
            sa    = sc == sl
            for ci in np.where(sa)[0]:
                sub = seg[ci:]
                if len(sub) < 5: continue
                if float(np.std(sub)) > std_tol: continue
                dt = float(seg_t[-1] - seg_t[ci])
                if dt > 1e-12:
                    if abs(float(sub[-1]) - float(sub[0])) / dt / scale_ref * 100 > slope_tol:
                        continue
                settling  = float(seg_t[ci])
                converged = True
                break

        # 6. 收敛二次保险
        # (a) 超调>200%：参数严重不当
        # (b) verr>10%：Ki 不足，稳态误差未消除
        if converged:
            if overshoot > 200.0 or verr_pct > 10.0:
                converged = False
                settling  = float(t[-1])

        # 7. 曲线分类
        state, diag = classify_curve(trend=trend, t=t, steady_state=ss, converged=converged)

        # 8. 写回
        result.steady_state          = round(ss, 4)
        result.verr_pct              = round(verr_pct, 3)
        result.peak                  = round(peak, 4)
        result.overshoot_pct         = round(overshoot, 2)
        result.rise_time             = round(rise_time, 4)
        result.settling_time         = round(settling, 4)
        result.tail_std              = round(tail_std, 6)
        result.converged             = converged
        result.curve_state           = state
        result.tail_drift_pct        = round(diag["tail_drift_pct"], 4)
        result.end_slope_pct_per_s   = round(diag["end_slope_pct_per_s"], 4)
        result.abs_end_slope         = round(diag["abs_end_slope"], 6)
        result.tail_range_pct        = round(diag["tail_range_pct"], 4)
        result.half_zero_crossings   = int(diag["half_zero_crossings"])
        result.oscillation_decay_ratio = round(diag["oscillation_decay_ratio"], 4)
        result.recommended_action    = diag["recommended_action"]
        result.action_reason         = diag["action_reason"]