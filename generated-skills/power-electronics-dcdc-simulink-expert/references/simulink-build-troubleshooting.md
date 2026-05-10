# Simulink 自动建模与批处理排错

用于 DC/DC 自动建模、MATLAB Function 块生成、`matlab -batch` 批处理仿真和报告生成。先记录真实错误，再按下面顺序修复；不要把失败仿真写成已验证结果。

## MATLAB 启动与权限

- 如果 MCP 报 `MATLAB session ... is not alive`，改用本地 `matlab -batch` 验证 MATLAB 是否可启动。
- 如果沙箱内 `matlab -batch` 出现 `Fatal Startup Error`、`File system inconsistency` 或类似文件系统/权限问题，按环境规则请求沙箱外执行，不要绕过审批。
- 批处理脚本应可重复运行：开头关闭已加载模型、删除或覆盖旧 `.slx`，输出目录不存在时创建。

## MATLAB Function 块代码生成

- 生成 MATLAB Function 块脚本时，避免把含 `%` 的代码片段直接交给 `sprintf`。`%#codegen`、报告百分号、注释百分号都可能触发 `格式无效`。
- 更稳的模式是先用 `strjoin([...], "")` 组成单个格式字符串，只保留真正的 `%.17g` 数值占位；不需要 `sprintf` 的百分号要写成 `%%`，或直接移除非必要注释。
- 如果后续要用 `SimulationInput` 独立运行模型，优先把本轮已验证的 PI 数值写入 MATLAB Function 块脚本，或明确配置 Model Workspace 数据；不要只依赖当前 base workspace。

## 输出维度与参数解析

- 如果编译报 `没有完全设置输出 ... 的维度`，在 MATLAB Function 入口立即给所有输出赋标量初值，例如：

```matlab
function [gate,d,i_ref] = fcn(t,vo,il)
persistent ev ei carrier
gate = 0; d = 0; i_ref = 0;
```

- 对功率级函数同样显式初始化输出：

```matlab
function [vo,il] = fcn(t,gate)
persistent x_vo x_il
vo = 0; il = 0;
```

- 如果解析 MATLAB Function 块时只提示块解析失败，先读取块脚本确认生成内容，再检查未定义的 `Kp_i`、`Ki_i`、`Kp_v`、`Ki_v` 等工作区符号。

## 代数环

- 控制器 MATLAB Function 和功率级 MATLAB Function 都含 `persistent` 状态时，直接反馈 `Vo/iL -> controller -> duty/gate -> plant -> Vo/iL` 容易形成代数环。
- 对数字控制 DC/DC，反馈采样延迟是合理建模。优先在 `Vo` 和 `iL` 反馈通道加入 `Unit Delay`，采样时间设为控制步长，初值设为 0。
- 加入延迟后再用 `Simulink.SimulationInput` 跑一次完整模型，确认停止事件为 `ReachedStopTime` 或其他可解释的正常停止条件。

## 报告与结果文件

- 中文 Markdown 报告用 UTF-8 写入：

```matlab
fid = fopen(fileName, "w", "n", "UTF-8");
```

- 保存 `.mat` 时同时保存参数、调参表、最佳候选、波形、指标和模型仿真停止事件，便于复核：

```matlab
save(resultFile, "p", "innerTable", "outerTable", "best", "wave", "metrics", "simStatus");
```

## 推荐验证顺序

1. `checkcode` 或 Code Analyzer 检查脚本语法。
2. 运行批处理脚本完成建模和参数搜索。
3. 用 `SimulationInput` 对生成的 `.slx` 独立仿真一次。
4. 读取 `.mat` 摘要并确认 PI 参数、指标和 `simStatus`。
5. 用 UTF-8 打开报告，确认中文正常、指标和文件路径完整。
