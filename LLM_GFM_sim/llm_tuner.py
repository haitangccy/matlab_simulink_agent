"""
llm_tuner.py  —  v3

管理调参历史、评分逻辑，以及下一轮动作决策。

主要修复（v3）：
  1. initial_params：Ki 从 10.0 出发（不再是 0），Buck 电压环
     纯 P 控制必然有稳态误差，Ki=0 是死局。
  2. __init__ 新增 initial_kp / initial_ki 参数，可在 main.py
     为内环、外环分别设置合适起点。
  3. score_metrics：overshoot>100% 时强力惩罚，>200% 时分数
     几乎归零，防止 21096% 超调仍被选为"最优"。
  4. score_metrics：加入 verr_pct 惩罚，稳态误差>2% 时扣分，
     >10% 时大幅扣分，引导 LLM 增大 Ki。
  5. _build_prompt：加入 Ki≠0 硬性约束 + verr_pct 显示，
     让 LLM 直接看到"距目标还差多少"。
  6. _rule_based_fallback：verr_pct>5% 时优先大幅增大 Ki，
     而不是去调 Kp，消除纯规则情况下的 Ki=0 陷阱。
  7. _check_stagnation：Ki 扰动方向单独控制，不跟随 Kp 反向
     （否则 Kp 减小时 Ki 也会被跟着减小）。
"""

import json
import re
import urllib.error
import urllib.request
from dataclasses import dataclass
from typing import Optional

import numpy as np


def _resolve_time_refs(
    settling_ref: Optional[float],
    rise_ref: Optional[float],
    base_stop_time: Optional[float] = None,
) -> tuple[float, float]:
    base = float(base_stop_time) if base_stop_time is not None else 1.0
    base = max(base, 1e-6)
    s = float(settling_ref) if settling_ref is not None else base
    s = max(s, 1e-6)
    r = float(rise_ref) if rise_ref is not None else max(s / 2.0, 1e-6)
    r = max(r, 1e-6)
    return s, r


# ---------------------------------------------------------------------------
# 数据类
# ---------------------------------------------------------------------------

@dataclass
class LLMConfig:
    api_url:     str   = "https://api.siliconflow.cn/v1/chat/completions"
    api_key:     str   = ""
    model_name:  str   = "deepseek-ai/DeepSeek-V3.2"
    timeout:     int   = 30
    max_tokens:  int   = 300
    temperature: float = 0.5


@dataclass
class TuningRecord:
    round_no: int
    kp: float
    ki: float
    metrics: dict
    reason:    str   = ""
    stop_time: float = 0.0
    is_probe:  bool  = False


@dataclass
class TuningBounds:
    kp_min: float = 0.0
    kp_max: float = float("inf")
    ki_min: float = 0.0
    ki_max: float = float("inf")

    def clip_kp(self, v: float) -> float:
        return float(np.clip(v, self.kp_min, self.kp_max if np.isfinite(self.kp_max) else 1e9))

    def clip_ki(self, v: float) -> float:
        return float(np.clip(v, self.ki_min, self.ki_max if np.isfinite(self.ki_max) else 1e9))


@dataclass
class TuningDecision:
    kp: float
    ki: float
    reason: str
    action:    str           = "retune_pi"
    stop_time: Optional[float] = None


# ---------------------------------------------------------------------------
# 评分函数
# ---------------------------------------------------------------------------

def score_metrics(
    metrics: dict,
    settling_ref: Optional[float] = None,
    rise_ref: Optional[float] = None,
    tail_ref: float = 0.05,
) -> float:
    """
    综合评分 [0, 1]，越高越好。

    修复（v3）：
    - overshoot > 100% 强力惩罚，> 200% 分数接近 0
    - 加入 verr_pct 惩罚（稳态误差），Ki 不够时自动扣分
    """
    curve_state  = metrics.get("curve_state", "converged")
    settling_ref, rise_ref = _resolve_time_refs(settling_ref, rise_ref)

    # ---- 上升中 ----
    if curve_state == "rising":
        drift  = metrics.get("tail_drift_pct", 100.0)
        slope  = abs(metrics.get("end_slope_pct_per_s", 100.0))
        ts     = metrics.get("tail_std", tail_ref)
        zc     = metrics.get("half_zero_crossings", 0)
        total  = (
            0.45 * max(0.0, 1.0 - drift / 8.0)
            + 0.35 * max(0.0, 1.0 - slope / 2.0)
            + 0.10 * max(0.0, 1.0 - ts / max(tail_ref, 1e-6))
            + 0.10 * (1.0 if zc <= 1 else max(0.0, 1.0 - zc / 6.0))
        )
        if metrics.get("recommended_action") == "extend_stop_time":
            total *= 0.95
        return round(total, 6)

    # ---- 振荡中 ----
    if curve_state == "oscillating":
        tr  = metrics.get("tail_range_pct", 100.0)
        dr  = metrics.get("oscillation_decay_ratio", 10.0)
        zc  = metrics.get("half_zero_crossings", 10)
        ts  = metrics.get("tail_std", tail_ref)
        total = (
            0.40 * max(0.0, 1.0 - tr / 15.0)
            + 0.30 * max(0.0, 1.0 - max(0.0, dr - 0.5) / 1.5)
            + 0.20 * max(0.0, 1.0 - zc / 12.0)
            + 0.10 * max(0.0, 1.0 - ts / max(tail_ref, 1e-6))
        )
        return round(total * 0.6, 6)

    # ---- 收敛 ----
    st  = metrics.get("settling_time", settling_ref)
    rt  = metrics.get("rise_time", rise_ref)
    ts  = metrics.get("tail_std", tail_ref)
    ov  = metrics.get("overshoot_pct", 0.0)
    ver = metrics.get("verr_pct", 0.0)

    st_score   = max(0.0, 1.0 - (st / settling_ref) ** 1.2)
    rise_score = max(0.0, 1.0 - rt / rise_ref)
    tail_score = max(0.0, 1.0 - ts / max(tail_ref, 1e-6))

    # 超调惩罚（三段）：
    #   0~20%   → 不惩罚
    #   20~100% → 线性 0~0.40
    #   >100%   → 强力惩罚，200% 时扣 0.85，防止浪涌被选为"最优"
    if ov <= 20.0:
        ov_pen = 0.0
    elif ov <= 100.0:
        ov_pen = (ov - 20.0) / 80.0 * 0.40
    else:
        ov_pen = 0.40 + min((ov - 100.0) / 100.0 * 0.45, 0.55)
    ov_pen = min(ov_pen, 0.95)

    # 稳态误差惩罚：Ki 不够时 verr_pct 大，直接扣分
    #   0~2%  → 不惩罚
    #   2~10% → 线性 0~0.30
    #   >10%  → 大幅惩罚，上限 0.60
    if ver <= 2.0:
        ver_pen = 0.0
    elif ver <= 10.0:
        ver_pen = (ver - 2.0) / 8.0 * 0.30
    else:
        ver_pen = 0.30 + min((ver - 10.0) / 20.0 * 0.30, 0.30)
    ver_pen = min(ver_pen, 0.60)

    total = max(0.0,
        0.55 * st_score
        + 0.20 * rise_score
        + 0.15 * tail_score
        - ov_pen
        - ver_pen
    )

    if curve_state not in ("converged",):
        total *= 0.3
    if metrics.get("recommended_action") == "extend_stop_time":
        total *= 0.9

    return round(total, 6)


# ---------------------------------------------------------------------------
# LLMTuner
# ---------------------------------------------------------------------------

class LLMTuner:
    def __init__(
        self,
        config: LLMConfig,
        bounds: TuningBounds,
        settling_ref: Optional[float]  = None,
        rise_ref:     Optional[float]  = None,
        tail_ref:     float            = 0.05,
        base_stop_time: Optional[float] = None,
        stop_time_growth: float        = 1.5,
        max_stop_time:    float        = 120.0,
        # ★ 新增：初始参数，供 main.py 为内环/外环分别指定合理起点
        initial_kp: float = 0.1,
        initial_ki: float = 10.0,
        # 目标电压/电流（传给 prompt，用于解释 verr_pct 含义）
        target_label: str = "",
    ):
        self.cfg    = config
        self.bounds = bounds
        self.history: list[TuningRecord]  = []
        self._best:   Optional[TuningRecord] = None

        self._settling_ref, self._rise_ref = _resolve_time_refs(settling_ref, rise_ref, base_stop_time)
        self._tail_ref         = tail_ref
        self._stop_time_growth = stop_time_growth
        self._max_stop_time    = max_stop_time
        self._base_stop_time   = self._settling_ref
        self._initial_kp       = float(initial_kp)
        self._initial_ki       = float(initial_ki)
        self._target_label     = target_label
        self._stagnation_count = 0

    @property
    def best(self) -> Optional[TuningRecord]:
        return self._best

    # ------------------------------------------------------------------
    # 公开接口
    # ------------------------------------------------------------------

    def initial_params(self) -> tuple[float, float]:
        """
        返回初始参数。

        ★ Ki 绝对不能从 0 出发：
          - Buck 电压环纯 P 控制必然有稳态误差
          - LLM 一旦看到"收敛"就会停止增大 Ki
          - 0 × 任何系数 = 0，规则 fallback 也永远出不了 Ki=0 的坑
        默认 (initial_kp, initial_ki)，在 main.py 中按环路特性覆盖。
        """
        return self._initial_kp, self._initial_ki

    def record(
        self,
        round_no: int,
        kp: float, ki: float,
        metrics: dict,
        reason: str   = "",
        stop_time: float = 0.0,
        is_probe: bool = False,
    ) -> TuningRecord:
        rec = TuningRecord(
            round_no=round_no, kp=kp, ki=ki,
            metrics=metrics, reason=reason,
            stop_time=stop_time, is_probe=is_probe,
        )
        self.history.append(rec)
        if not is_probe:
            self._update_best(rec)
        return rec

    def suggest_next(self, current_stop_time: float) -> TuningDecision:
        """决定下一轮参数或是否延长仿真时长。"""
        if not self.history:
            kp, ki = self.initial_params()
            return TuningDecision(kp=kp, ki=ki, reason="初始参数")

        last_non_probe = next(
            (r for r in reversed(self.history) if not r.is_probe),
            self.history[-1],
        )
        extend = self._try_extend_stop_time(last_non_probe.metrics, current_stop_time)
        if extend is not None:
            return extend

        # 调用 LLM，失败时走规则 fallback
        try:
            kp, ki, reason = self._call_llm()
        except Exception as e:
            print(f"  [LLMTuner] LLM 调用失败（{e}），使用规则回退")
            kp, ki, reason = self._rule_based_fallback()

        # Ki 保底：不允许小于初始 Ki 的 1%
        ki = max(ki, self._initial_ki * 0.01)

        decision = TuningDecision(
            kp=self.bounds.clip_kp(kp),
            ki=self.bounds.clip_ki(ki),
            reason=reason,
        )
        self._check_stagnation(decision)
        return decision

    # ------------------------------------------------------------------
    # 停滞检测
    # ------------------------------------------------------------------

    def _check_stagnation(self, decision: TuningDecision) -> None:
        if len(self.history) < 3:
            self._stagnation_count = 0
            return
        recent = [self._score(r.metrics) for r in self.history[-3:] if not r.is_probe]
        if len(recent) < 3:
            return
        if max(recent) - min(recent) < 0.02:
            self._stagnation_count += 1
        else:
            self._stagnation_count = 0
        if self._stagnation_count >= 2:
            kp_noise = float(np.random.choice([-1, 1])) * float(np.random.uniform(0.4, 0.8))
            ki_noise = float(np.random.uniform(0.3, 0.7))   # Ki 只增不随 Kp 反向
            decision.kp = self.bounds.clip_kp(decision.kp * np.exp(kp_noise))
            decision.ki = self.bounds.clip_ki(decision.ki * np.exp(ki_noise))
            decision.reason += f"（停滞{self._stagnation_count}轮，强制大步跳跃）"
            print(f"  [LLMTuner] 停滞跳跃 → Kp={decision.kp:.3f}, Ki={decision.ki:.3f}")
            self._stagnation_count = 0

    # ------------------------------------------------------------------
    # 最优记录
    # ------------------------------------------------------------------

    def _score(self, m: dict) -> float:
        return score_metrics(m, self._settling_ref, self._rise_ref, self._tail_ref)

    def _update_best(self, rec: TuningRecord) -> None:
        if self._best is None:
            self._best = rec
            print(f"  [LLMTuner] 第{rec.round_no}轮成为初始最优, score={self._score(rec.metrics):.4f}")
            return
        cs = self._score(rec.metrics)
        bs = self._score(self._best.metrics)
        if cs > bs:
            self._best = rec
            print(f"  [LLMTuner] 第{rec.round_no}轮更新最优: {bs:.4f} → {cs:.4f}")

    # ------------------------------------------------------------------
    # 延长仿真决策
    # ------------------------------------------------------------------

    def _has_probed(self) -> bool:
        return any(r.is_probe for r in self.history)

    def _try_extend_stop_time(self, metrics: dict, cur: float) -> Optional[TuningDecision]:
        if metrics.get("recommended_action") != "extend_stop_time":
            return None
        if self._has_probed():
            print("  [LLMTuner] 已做过延长探测，继续调 PI")
            return None
        nxt = min(cur * self._stop_time_growth, self._max_stop_time)
        if nxt <= cur + 1e-9:
            return None
        last = self.history[-1]
        return TuningDecision(
            kp=last.kp, ki=last.ki,
            reason=metrics.get("action_reason", "延长仿真确认收敛"),
            action="extend_stop_time", stop_time=round(nxt, 4),
        )

    # ------------------------------------------------------------------
    # Prompt 构建
    # ------------------------------------------------------------------

    def _metrics_summary(self, m: dict) -> str:
        s = m.get("curve_state", "unknown")
        ver = m.get("verr_pct", 0.0)
        ver_str = f", verr_pct={ver}%" if ver > 0.1 else ""
        if s == "rising":
            return (f"tail_std={m.get('tail_std')}, tail_drift={m.get('tail_drift_pct')}, "
                    f"slope={m.get('end_slope_pct_per_s')}, zc={m.get('half_zero_crossings')}{ver_str}")
        if s == "oscillating":
            return (f"tail_std={m.get('tail_std')}, tail_range={m.get('tail_range_pct')}, "
                    f"zc={m.get('half_zero_crossings')}, decay={m.get('oscillation_decay_ratio')}{ver_str}")
        return (f"overshoot={m.get('overshoot_pct')}%, settling={m.get('settling_time')}s, "
                f"rise={m.get('rise_time')}s, tail_std={m.get('tail_std')}, "
                f"tail_drift={m.get('tail_drift_pct')}{ver_str}")

    def _build_prompt(self) -> str:
        b = self.bounds
        kp_max_str = f"{b.kp_max}" if np.isfinite(b.kp_max) else "∞"
        ki_max_str = f"{b.ki_max}" if np.isfinite(b.ki_max) else "∞"

        recent = [r for r in self.history if not r.is_probe][-5:]
        scores = [self._score(r.metrics) for r in recent]
        improving = len(scores) >= 2 and scores[-1] > scores[0]

        stag_hint = ""
        if self._stagnation_count >= 1:
            stag_hint = "\n⚠️  调参停滞，请大幅改变方向（乘或除以 2～5 倍），不要微调！"

        target_hint = f"  目标输出值：{self._target_label}\n" if self._target_label else ""

        lines = [
            "你是一个 Buck 变换器双环 PI 调参专家。",
            f"目标：在 0%~10% 超调范围内尽量缩短 settling_time，同时消除稳态误差（verr_pct < 1%）。",
            "",
            f"参数约束：Kp ∈ [{b.kp_min}, {kp_max_str}]，Ki ∈ [{b.ki_min}, {ki_max_str}]",
            "",
            "硬性约束（必须遵守）：",
            "  ❌ Ki 绝对不能为 0！Buck 双环纯 P 控制必然有稳态误差，Ki 最小建议 ≥ 1.0",
            "  ❌ 不允许建议 Ki < 0.5",
            "",
            target_hint,
            "调参策略：",
            "  · verr_pct > 2%（输出偏离目标）→ 增大 Ki（×1.5～×3），这是消稳态误差的唯一方法",
            "  · verr_pct > 5% 且 Ki 很小 → 大幅增大 Ki（×3～×10）",
            "  · tail_drift 大（>8）→ 增大 Kp（×2～×4），Ki 小幅跟随",
            "  · 振荡 → 减小 Kp（×0.3～×0.6），适度减小 Ki（×0.6～×0.8）",
            "  · 超调 >20% → 减小 Kp（×0.7～×0.9）",
            "  · settling 仍长且已收敛 → 同时增大 Kp 和 Ki（×1.3～×2）",
            "  · 避免微调 ±10%，效果太慢！",
            stag_hint,
            "",
            "历史记录（非探测轮）：",
        ]

        for rec in recent:
            m = rec.metrics
            lines.append(
                f"  第{rec.round_no}轮: Kp={rec.kp}, Ki={rec.ki}, "
                f"score={self._score(m):.4f}, state={m.get('curve_state')}, "
                f"{self._metrics_summary(m)}"
            )

        trend_str = "上升中" if improving else "未改善"
        lines += [
            f"\n近期趋势：{trend_str}，最新得分={scores[-1]:.4f}" if scores else "",
            "",
            '只返回 JSON，格式：{"kp": float, "ki": float, "reason": "简要说明"}',
            "reason 中说明本次调整方向及理由。",
        ]
        return "\n".join(l for l in lines if l is not None)

    # ------------------------------------------------------------------
    # LLM 调用
    # ------------------------------------------------------------------

    def _call_llm(self) -> tuple[float, float, str]:
        prompt  = self._build_prompt()
        payload = json.dumps({
            "model":       self.cfg.model_name,
            "messages":    [{"role": "user", "content": prompt}],
            "max_tokens":  self.cfg.max_tokens,
            "temperature": self.cfg.temperature,
        }).encode("utf-8")
        headers = {"Content-Type": "application/json"}
        if self.cfg.api_key:
            headers["Authorization"] = f"Bearer {self.cfg.api_key}"
        req = urllib.request.Request(self.cfg.api_url, data=payload, headers=headers, method="POST")
        with urllib.request.urlopen(req, timeout=self.cfg.timeout) as resp:
            raw = resp.read().decode("utf-8")
        outer = json.loads(raw)
        if "choices" in outer:
            text = outer["choices"][0]["message"]["content"]
        elif "output" in outer and isinstance(outer["output"], dict):
            text = outer["output"].get("text", raw)
        else:
            text = raw
        text  = text.strip()
        match = re.search(r"\{.*?\}", text, re.DOTALL)
        res   = json.loads(match.group(0) if match else text)
        kp    = float(res.get("kp", res.get("Kp", self._initial_kp)))
        ki    = float(res.get("ki", res.get("Ki", self._initial_ki)))
        return kp, ki, res.get("reason", "")

    # ------------------------------------------------------------------
    # 规则回退
    # ------------------------------------------------------------------

    def _rule_based_fallback(self) -> tuple[float, float, str]:
        last = next((r for r in reversed(self.history) if not r.is_probe), self.history[-1])
        kp   = last.kp
        ki   = max(last.ki, self._initial_ki * 0.1)   # Ki 保底，0 × n = 0 的陷阱
        m    = last.metrics
        state  = m.get("curve_state", "converged")
        verr   = m.get("verr_pct", 0.0)

        # ★ verr_pct 优先：稳态误差大时，Ki 是第一优先级
        if verr > 5.0:
            ki   *= float(np.random.uniform(3.0, 6.0))
            reason = f"规则回退：verr_pct={verr:.1f}%，大幅增大 Ki 消除稳态误差"
            return self.bounds.clip_kp(kp), self.bounds.clip_ki(ki), reason
        if verr > 2.0:
            ki   *= float(np.random.uniform(1.5, 3.0))
            reason = f"规则回退：verr_pct={verr:.1f}%，增大 Ki"
            return self.bounds.clip_kp(kp), self.bounds.clip_ki(ki), reason

        if state == "oscillating":
            kp *= float(np.random.uniform(0.3, 0.55))
            ki *= float(np.random.uniform(0.55, 0.75))
            reason = "规则回退：振荡，大幅减 Kp，适度减 Ki"
        elif state == "rising":
            drift = m.get("tail_drift_pct", 100.0)
            if drift >= 8.0:
                kp *= float(np.random.uniform(2.0, 4.0))
                ki *= float(np.random.uniform(1.1, 1.4))
                reason = "规则回退：尾部大幅漂移，大步增 Kp"
            elif drift >= 3.0:
                kp *= float(np.random.uniform(1.4, 2.0))
                ki *= float(np.random.uniform(1.1, 1.3))
                reason = "规则回退：中等漂移，中步增 Kp"
            else:
                ki *= float(np.random.uniform(1.3, 2.0))
                reason = "规则回退：小幅上升，增大 Ki"
        else:
            ov = m.get("overshoot_pct", 0.0)
            st = m.get("settling_time", self._settling_ref)
            if ov > 20.0:
                kp *= float(np.random.uniform(0.6, 0.8))
                reason = "规则回退：超调偏大，减小 Kp"
            elif st > 0.7 * self._settling_ref:
                kp *= float(np.random.uniform(1.3, 2.0))
                ki *= float(np.random.uniform(1.3, 2.0))
                reason = "规则回退：settling 偏长，增大 Kp 和 Ki"
            else:
                kp *= float(np.exp(np.random.randn() * 0.25))
                ki *= float(np.exp(np.random.randn() * 0.25))
                reason = "规则回退：已接近最优，随机扰动"

        return self.bounds.clip_kp(kp), self.bounds.clip_ki(ki), reason