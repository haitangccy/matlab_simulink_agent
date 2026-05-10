import os

import matplotlib as mpl

from buck_agent import BuckTuningAgent, build_arg_parser, parse_agent_request
from llm_tuner import LLMConfig


mpl.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei", "SimSun"]
mpl.rcParams["axes.unicode_minus"] = False


def build_llm_config() -> LLMConfig:
    return LLMConfig(
        api_url=os.getenv("SILICONFLOW_API_URL", "https://api.siliconflow.cn/v1/chat/completions"),
        api_key=os.getenv("SILICONFLOW_API_KEY", ""),
        model_name=os.getenv("SILICONFLOW_MODEL", "deepseek-ai/DeepSeek-V3.2"),
    )


def main() -> None:
    parser = build_arg_parser()
    args = parser.parse_args()

    request = parse_agent_request(
        args.request,
        target_voltage=args.target_voltage,
        control_mode=args.control_mode,
        inner_rounds=args.inner_rounds,
        outer_rounds=args.outer_rounds,
        generate_report=not args.no_report,
        show_plot=not args.no_show,
        output_dir=args.output_dir,
    )

    print("=" * 60)
    print("Buck 调参 Agent 已启动")
    print("=" * 60)
    print(f"需求: {request.raw_text}")
    print(f"识别结果: target_voltage={request.target_voltage}V, control_mode={request.control_mode}")

    agent = BuckTuningAgent(build_llm_config())
    artifacts = agent.run(request)

    print("\n" + "=" * 60)
    print("任务完成")
    print("=" * 60)
    print(
        f"内环最优: Kp={artifacts.best_inner['kp']:.4f}, Ki={artifacts.best_inner['ki']:.4f}\n"
        f"外环最优: Kp={artifacts.best_outer['kp']:.4f}, Ki={artifacts.best_outer['ki']:.4f}"
    )
    print(
        f"最终指标: steady_state={artifacts.final_metrics['steady_state']}V, "
        f"verr={artifacts.final_metrics['verr_pct']}%, "
        f"settling={artifacts.final_metrics['settling_time']}s, "
        f"overshoot={artifacts.final_metrics['overshoot_pct']}%"
    )
    if artifacts.figure_path is not None:
        print(f"结果图: {artifacts.figure_path}")
    if artifacts.report_path is not None:
        print(f"报告: {artifacts.report_path}")


if __name__ == "__main__":
    main()
