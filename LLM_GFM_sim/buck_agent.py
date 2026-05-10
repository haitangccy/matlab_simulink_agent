import argparse
import json
import re
from dataclasses import asdict, dataclass
from datetime import datetime
from pathlib import Path
from typing import Any, Optional, TYPE_CHECKING

import matplotlib.pyplot as plt

from llm_tuner import LLMConfig, LLMTuner, TuningBounds

if TYPE_CHECKING:
    from vsg_simulator import SimResult


INNER_STOP_TIME = 0.005
OUTER_STOP_TIME = 0.05
BASE_STOP_TIME = OUTER_STOP_TIME

DEFAULT_MODEL_NAME = "ph2_interleaved_BuckVi48"


@dataclass
class AgentRequest:
    raw_text: str
    circuit_type: str = "buck"
    control_mode: str = "dual_loop"
    target_voltage: float = 12.0
    inner_rounds: int = 10
    outer_rounds: int = 10
    generate_report: bool = True
    show_plot: bool = True
    output_dir: str = "agent_outputs"


@dataclass
class AgentArtifacts:
    request: AgentRequest
    output_dir: Path
    figure_path: Optional[Path]
    report_path: Optional[Path]
    final_metrics: dict
    best_inner: dict
    best_outer: dict
    final_stop_time: float


@dataclass(frozen=True)
class ModelSignalConfig:
    model_name: str
    controller_block_i_1: str
    controller_block_i_2: str
    controller_block_e_1: str
    controller_block_e_2: str
    out_signal: str
    voltage_signal: Optional[str]

DEFAULT_MODEL_CONFIG = ModelSignalConfig(
    model_name=DEFAULT_MODEL_NAME,
    controller_block_i_1="PID_Controller_i_1",
    controller_block_i_2="PID_Controller_i_2",
    controller_block_e_1="PID_Controller_e_1",
    controller_block_e_2="PID_Controller_e_2",
    out_signal="out.Data.Data",
    voltage_signal="out.V.Data",
)


def parse_agent_request(
    raw_text: str,
    *,
    target_voltage: Optional[float] = None,
    control_mode: Optional[str] = None,
    inner_rounds: int = 10,
    outer_rounds: int = 10,
    generate_report: bool = True,
    show_plot: bool = True,
    output_dir: str = "agent_outputs",
) -> AgentRequest:
    text = raw_text.strip()
    voltage = target_voltage if target_voltage is not None else _extract_voltage(text) or 12.0
    mode = control_mode or _extract_control_mode(text)
    return AgentRequest(
        raw_text=text or f"Buck 电路目标电压 {voltage}V，{mode}",
        circuit_type="buck",
        control_mode=mode,
        target_voltage=voltage,
        inner_rounds=inner_rounds,
        outer_rounds=outer_rounds,
        generate_report=generate_report,
        show_plot=show_plot,
        output_dir=output_dir,
    )


def _extract_voltage(text: str) -> Optional[float]:
    match = re.search(r"(\d+(?:\.\d+)?)\s*[vV伏]", text)
    if match:
        return float(match.group(1))
    match = re.search(r"目标电压[^\d]*(\d+(?:\.\d+)?)", text)
    if match:
        return float(match.group(1))
    return None


def _extract_control_mode(text: str) -> str:
    normalized = text.lower()
    if any(token in normalized for token in ("双环", "双闭环", "cascaded", "dual loop", "dual_loop")):
        return "dual_loop"
    if any(token in normalized for token in ("单环", "单闭环", "single loop", "single_loop")):
        return "single_loop"
    return "dual_loop"


def print_metrics(metrics: dict) -> None:
    state = metrics["curve_state"]
    action = metrics.get("recommended_action", "?")
    reason = metrics.get("action_reason", "")
    ver = metrics.get("verr_pct", 0.0)
    ss = metrics.get("steady_state", 0.0)
    ver_str = f", V0={ss}V, verr={ver:.2f}%" if ver > 0 else ""

    if state == "rising":
        print(
            f"  [rising] tail_drift={metrics['tail_drift_pct']:.2f}%, "
            f"slope={metrics['end_slope_pct_per_s']:.3f}%/s, tail_std={metrics['tail_std']:.4f}, "
            f"tail_range={metrics['tail_range_pct']:.2f}%, zero_x={metrics['half_zero_crossings']}"
            f"{ver_str} | -> {action}: {reason}"
        )
    elif state == "oscillating":
        print(
            f"  [oscillating] tail_range={metrics['tail_range_pct']:.2f}%, "
            f"decay={metrics['oscillation_decay_ratio']:.3f}, zero_x={metrics['half_zero_crossings']}, "
            f"tail_std={metrics['tail_std']:.4f}{ver_str} | -> {action}: {reason}"
        )
    else:
        print(
            f"  [converged] overshoot={metrics.get('overshoot_pct', 0):.1f}%, "
            f"settling={metrics.get('settling_time', 0):.4f}s, rise={metrics.get('rise_time', 0):.4f}s, "
            f"tail_std={metrics['tail_std']:.4f}, tail_drift={metrics['tail_drift_pct']:.2f}%"
            f"{ver_str} | -> {action}: {reason}"
        )


def print_stage_header(name: str) -> None:
    print("\n" + "=" * 60)
    print(f"  {name}")
    print("=" * 60)


def print_round_header(stage: str, rnd: int, total: int, stop_time: float) -> None:
    print(f"\n{'-' * 60}")
    print(f"{stage}  第 {rnd}/{total} 轮  |  仿真时长={stop_time}s")


def tune_stage(
    sim,
    tuner: LLMTuner,
    num_rounds: int,
    stage_name: str,
    run_once,
):
    base_stop_time = sim.cfg.stop_time
    active_stop_time = base_stop_time
    last_was_probe = False

    print_stage_header(stage_name)

    for rnd in range(1, num_rounds + 1):
        if last_was_probe and active_stop_time != base_stop_time:
            active_stop_time = base_stop_time
            sim.update_stop_time(active_stop_time)
            print(f"  <- 已恢复基准时长={active_stop_time}s，继续调 PI")
        last_was_probe = False

        print_round_header(stage_name, rnd, num_rounds, active_stop_time)

        if rnd == 1:
            kp, ki = tuner.initial_params()
            reason, is_probe = "初始参数", False
        else:
            decision = tuner.suggest_next(active_stop_time)
            kp, ki = decision.kp, decision.ki
            reason = decision.reason
            is_probe = decision.action == "extend_stop_time"

            if is_probe and decision.stop_time is not None:
                last_rec = next(
                    (r for r in reversed(tuner.history) if not r.is_probe),
                    tuner.history[-1],
                )
                kp, ki = last_rec.kp, last_rec.ki
                active_stop_time = decision.stop_time
                sim.update_stop_time(active_stop_time)
                print(f"  -> 延长仿真时长 {active_stop_time}s（{reason}）")
            else:
                print(f"  * 新参数：Kp={kp:.4f}, Ki={ki:.4f}（{reason}）")

        result = run_once(kp, ki)
        if not result.is_valid():
            print("  x 仿真结果无效，跳过")
            continue

        metrics = result.metrics_dict()
        print_metrics(metrics)

        tuner.record(
            round_no=rnd,
            kp=kp,
            ki=ki,
            metrics=result.decision_metrics(),
            reason=reason,
            stop_time=active_stop_time,
            is_probe=is_probe,
        )
        last_was_probe = is_probe

    best = tuner.best
    if best is None:
        raise RuntimeError(f"{stage_name} 未产生有效结果")

    print(f"\n{'=' * 60}")
    print(f"{stage_name} 完成  最优：Kp={best.kp:.4f}, Ki={best.ki:.4f}")
    print_metrics(best.metrics)
    return best


class BuckTuningAgent:
    def __init__(
        self,
        llm_cfg: LLMConfig,
        *,
        model_config: ModelSignalConfig = DEFAULT_MODEL_CONFIG,
        simulation_mode: str = "accelerator",
    ):
        self.llm_cfg = llm_cfg
        self.model_config = model_config
        self.simulation_mode = simulation_mode
        self.inner_bounds = TuningBounds(kp_min=0, kp_max=float("inf"), ki_min=0, ki_max=float("inf"))
        self.outer_bounds = TuningBounds(kp_min=0, kp_max=float("inf"), ki_min=0, ki_max=float("inf"))

    def run(self, request: AgentRequest) -> AgentArtifacts:
        from vsg_simulator import SimConfig, VSGSimulator

        if request.control_mode != "dual_loop":
            raise NotImplementedError("当前模型只支持 Buck 双环结构，请将 control_mode 设为 dual_loop。")

        output_dir = Path(request.output_dir)
        output_dir.mkdir(parents=True, exist_ok=True)

        sim_cfg = SimConfig(
            model_name=self.model_config.model_name,
            controller_block_i_1=self.model_config.controller_block_i_1,
            controller_block_i_2=self.model_config.controller_block_i_2,
            controller_block_e_1=self.model_config.controller_block_e_1,
            controller_block_e_2=self.model_config.controller_block_e_2,
            out_signal=self.model_config.out_signal,
            voltage_signal=self.model_config.voltage_signal,
            stop_time=BASE_STOP_TIME,
            simulation_mode=self.simulation_mode,
            target_value=request.target_voltage,
        )

        inner_tuner = LLMTuner(
            self.llm_cfg,
            self.inner_bounds,
            base_stop_time=BASE_STOP_TIME,
            initial_kp=0.5,
            initial_ki=100.0,
            target_label="内环电流参考值，目标是让内环快速稳定",
        )
        outer_tuner = LLMTuner(
            self.llm_cfg,
            self.outer_bounds,
            base_stop_time=BASE_STOP_TIME,
            initial_kp=0.1,
            initial_ki=20.0,
            target_label=f"目标输出电压 {request.target_voltage}V",
        )

        with VSGSimulator(sim_cfg) as sim:
            sim.update_stop_time(INNER_STOP_TIME)
            best_inner = tune_stage(
                sim=sim,
                tuner=inner_tuner,
                num_rounds=request.inner_rounds,
                stage_name="内环调参（电流环）",
                run_once=lambda kp, ki: sim.run_inner(kp_i=kp, ki_i=ki),
            )

            sim.update_stop_time(OUTER_STOP_TIME)
            best_outer = tune_stage(
                sim=sim,
                tuner=outer_tuner,
                num_rounds=request.outer_rounds,
                stage_name="外环调参（电压环）",
                run_once=lambda kp, ki: sim.run_outer(
                    kp_i=best_inner.kp,
                    ki_i=best_inner.ki,
                    kp_e=kp,
                    ki_e=ki,
                ),
            )

            final_stop_time = max(best_inner.stop_time, best_outer.stop_time, OUTER_STOP_TIME)
            sim.update_stop_time(final_stop_time)
            final = sim.run_outer(
                kp_i=best_inner.kp,
                ki_i=best_inner.ki,
                kp_e=best_outer.kp,
                ki_e=best_outer.ki,
            )
            if not final.is_valid():
                raise RuntimeError("最终验证仿真结果无效")

        figure_path = self._save_plot(request, final, best_inner, best_outer, output_dir)
        report_path = self._write_report(
            request=request,
            output_dir=output_dir,
            final=final,
            best_inner=best_inner,
            best_outer=best_outer,
            inner_tuner=inner_tuner,
            outer_tuner=outer_tuner,
            figure_path=figure_path,
            final_stop_time=final_stop_time,
        ) if request.generate_report else None

        if request.show_plot:
            plt.show()
        else:
            plt.close("all")

        return AgentArtifacts(
            request=request,
            output_dir=output_dir,
            figure_path=figure_path,
            report_path=report_path,
            final_metrics=final.metrics_dict(),
            best_inner={"kp": best_inner.kp, "ki": best_inner.ki, "stop_time": best_inner.stop_time},
            best_outer={"kp": best_outer.kp, "ki": best_outer.ki, "stop_time": best_outer.stop_time},
            final_stop_time=final_stop_time,
        )

    def _save_plot(
        self,
        request: AgentRequest,
        final: "SimResult",
        best_inner,
        best_outer,
        output_dir: Path,
    ) -> Path:
        has_voltage = len(final.v_data) > 0
        nrows = 2 if has_voltage else 1
        fig, axes = plt.subplots(nrows, 1, figsize=(11, 4 * nrows), sharex=True)
        if nrows == 1:
            axes = [axes]

        axes[0].plot(final.t, final.data, linewidth=0.8, color="#FF9800", label="内环PI输出")
        axes[0].set_ylabel("PI 输出")
        axes[0].set_title(
            f"内环 Kp={best_inner.kp:.3f} Ki={best_inner.ki:.3f}  |  "
            f"外环 Kp={best_outer.kp:.3f} Ki={best_outer.ki:.3f}",
            fontsize=10,
        )
        axes[0].legend(loc="upper right")
        axes[0].grid(True, alpha=0.4)

        if has_voltage:
            metrics = final.metrics_dict()
            axes[1].plot(final.t, final.v_data, linewidth=1.2, color="#2196F3", label="输出电压")
            axes[1].axhline(
                request.target_voltage,
                color="#F44336",
                linewidth=1.0,
                linestyle="--",
                label=f"目标 {request.target_voltage}V",
            )
            axes[1].set_xlabel("时间 (s)")
            axes[1].set_ylabel("电压 (V)")
            axes[1].set_title(
                f"V_steady={metrics['steady_state']}  verr={metrics['verr_pct']:.2f}%  "
                f"settling={metrics.get('settling_time', 'N/A')}s  "
                f"overshoot={metrics.get('overshoot_pct', 0):.1f}%",
                fontsize=10,
            )
            axes[1].legend(loc="lower right")
            axes[1].grid(True, alpha=0.4)

        plt.tight_layout()
        stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        figure_path = output_dir / f"buck_response_{stamp}.png"
        fig.savefig(figure_path, dpi=160, bbox_inches="tight")
        return figure_path

    def _write_report(
        self,
        *,
        request: AgentRequest,
        output_dir: Path,
        final: "SimResult",
        best_inner,
        best_outer,
        inner_tuner: LLMTuner,
        outer_tuner: LLMTuner,
        figure_path: Optional[Path],
        final_stop_time: float,
    ) -> Path:
        stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        report_path = output_dir / f"buck_report_{stamp}.md"
        final_metrics = final.metrics_dict()
        payload_path = output_dir / f"buck_summary_{stamp}.json"
        payload_path.write_text(
            json.dumps(
                {
                    "request": asdict(request),
                    "best_inner": {"kp": best_inner.kp, "ki": best_inner.ki, "stop_time": best_inner.stop_time},
                    "best_outer": {"kp": best_outer.kp, "ki": best_outer.ki, "stop_time": best_outer.stop_time},
                    "final_metrics": final_metrics,
                    "inner_history": [_record_to_dict(r) for r in inner_tuner.history],
                    "outer_history": [_record_to_dict(r) for r in outer_tuner.history],
                },
                ensure_ascii=False,
                indent=2,
            ),
            encoding="utf-8",
        )

        lines = [
            "# Buck 双环调参报告",
            "",
            "## 1. 任务概述",
            f"- 原始需求：{request.raw_text}",
            f"- 电路类型：{request.circuit_type}",
            f"- 控制结构：{request.control_mode}",
            f"- 目标输出电压：{request.target_voltage} V",
            f"- 最终验证仿真时长：{final_stop_time} s",
            "",
            "## 2. Agent 执行流程",
            "1. 解析输入需求，识别目标电压和双环控制结构。",
            "2. 初始化 Simulink 模型、PI 控制器块路径、目标电压和仿真时长。",
            "3. 先执行内环调参，让电流环先具备足够快的动态响应。",
            "4. 固定内环最优参数后执行外环调参，让输出电压跟踪目标值。",
            "5. 对最终参数做整体验证，并输出指标和结果图。",
            "",
            "## 3. 最优参数",
            f"- 内环 PI：Kp={best_inner.kp:.6f}, Ki={best_inner.ki:.6f}",
            f"- 外环 PI：Kp={best_outer.kp:.6f}, Ki={best_outer.ki:.6f}",
            "",
            "## 4. 最终指标",
            f"- 稳态值：{final_metrics['steady_state']} V",
            f"- 稳态误差：{final_metrics['verr_pct']} %",
            f"- 超调量：{final_metrics['overshoot_pct']} %",
            f"- 上升时间：{final_metrics['rise_time']} s",
            f"- 调节时间：{final_metrics['settling_time']} s",
            f"- 曲线状态：{final_metrics['curve_state']}",
            "",
            "## 5. 结构实现说明",
            "该工程当前采用 Buck 双环结构：内环控制电流、外环控制电压。",
            "实现上由 `VSGSimulator` 封装 Simulink 仿真调用，由 `LLMTuner` 负责根据历史指标给出下一轮 PI 参数。",
            "新增的 agent 层负责把自然语言需求转换为一组可执行配置，并把调参、验证、出图和报告串起来。",
            "",
            "## 6. 调参过程摘要",
            f"- 内环有效轮数：{len([r for r in inner_tuner.history if not r.is_probe])}",
            f"- 外环有效轮数：{len([r for r in outer_tuner.history if not r.is_probe])}",
            f"- 内环最优评分：{_best_score(inner_tuner):.6f}",
            f"- 外环最优评分：{_best_score(outer_tuner):.6f}",
            "",
            "## 7. 结果文件",
            f"- 响应图：{figure_path.name if figure_path else '未生成'}",
            f"- JSON 摘要：{payload_path.name}",
        ]
        report_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
        return report_path


def _record_to_dict(record: Any) -> dict:
    return {
        "round_no": record.round_no,
        "kp": record.kp,
        "ki": record.ki,
        "metrics": record.metrics,
        "reason": record.reason,
        "stop_time": record.stop_time,
        "is_probe": record.is_probe,
    }


def _best_score(tuner: LLMTuner) -> float:
    if tuner.best is None:
        return 0.0
    return tuner._score(tuner.best.metrics)


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Buck 双环调参 Agent")
    parser.add_argument(
        "--request",
        default="Buck 电路目标电压 12V，双环结构，自动给出数值指标、结果图和报告",
        help="自然语言任务描述",
    )
    parser.add_argument("--target-voltage", type=float, default=None, help="显式指定目标电压")
    parser.add_argument(
        "--control-mode",
        choices=["dual_loop", "single_loop"],
        default=None,
        help="控制结构",
    )
    parser.add_argument("--inner-rounds", type=int, default=10, help="内环调参轮数")
    parser.add_argument("--outer-rounds", type=int, default=10, help="外环调参轮数")
    parser.add_argument("--output-dir", default="agent_outputs", help="报告和图片输出目录")
    parser.add_argument("--no-report", action="store_true", help="不生成 Markdown 报告")
    parser.add_argument("--no-show", action="store_true", help="不弹出图窗，只保存图片")
    return parser
