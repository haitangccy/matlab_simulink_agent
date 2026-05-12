# DC/DC 基础设计规则

## 参数不足时

输入不完整时，继续推进，但要明确标注假设。只追问会影响下一步工程判断的关键值：

- 输入电压范围：`Vin_min`、`Vin_nom`、`Vin_max`
- 输出目标：`Vo`、`Po` 或 `Rload`
- 拓扑：Buck、Boost、Buck-Boost、半桥 LLC、全桥 LLC
- 开关频率或目标谐振频率
- 纹波指标、效率目标、动态响应目标
- 控制方式：占空比控制、电流模式控制、平均电流控制、LLC 调频控制

## Buck CCM 初步设计

以下公式只用于初选参数：

```text
D ~= Vo / Vin
Io = Po / Vo
R = Vo^2 / Po
Delta_iL ~= (Vin - Vo) * D / (L * fs)
Delta_Vo ~= Delta_iL / (8 * C * fs)
```

电感电流纹波可以先按额定输出电流的 20% 到 40% 估算，再根据电流应力、动态响应和器件体积约束调整。

必须在 `Vin_min`、`Vin_nom`、`Vin_max`、轻载、额定负载和负载阶跃下验证。模型支持时，要考虑 ESR、开关管压降、二极管压降或同步整流行为、采样/PWM 延迟和电流采样滤波。

## Boost 与 Buck-Boost

不要直接套 Buck 的控制直觉。连续导通模式 Boost 和反相 Buck-Boost 可能存在右半平面零点，电压外环通常要比同功率 Buck 更慢，尤其在高占空比工况下。

初步关系：

```text
Boost:      D ~= 1 - Vin / Vo
BuckBoost:  |Vo| ~= D / (1 - D) * Vin
```

这些关系用于可行性和应力初查，不用于最终控制器设计。

## 仿真是设计的一部分

计算值只是起点。最终结果取决于：

- 寄生参数和 ESR
- PWM 与采样延迟
- 求解器步长
- 电压/电流测量滤波
- 限幅与抗积分饱和
- 启动过程
- 负载和输入扰动

只要存在 Simulink 模型，就应尽快从公式计算切换到闭环仿真验证。

## Simulink 物理电路建模要求

对 Buck、Boost、Buck-Boost 等 DC/DC 电力电子电路，若用户要求“设计电路”“用电力电子器件”“Simulink 仿真模型”“物理模型”或要交付 `.slx`，功率级应优先搭建为物理电路，而不是只给平均模型：

- 输入侧：DC Voltage Source 或等效电源，必要时加入源阻抗、输入电容或输入扰动源。
- 开关器件：MOSFET/IGBT/Ideal Switch、二极管或同步整流 MOSFET，并设置导通电阻、压降、吸收网络等关键非理想参数。
- 储能与负载：电感、电容、ESR、负载电阻/动态负载按设计值连接。
- 测量与控制接口：输出电压、电感电流、开关电流、占空比/门极信号需要可记录到 `logsout`、`out` 或 `To Workspace`。
- 求解器：Specialized Power Systems 模型应包含 `powergui`；Simscape 模型应包含 Solver Configuration 和 Electrical Reference。步长要能解析 PWM 开关波形。

平均模型或传递函数模型可以用于公式校核和控制器初筛，但最终设计结论、PI 参数和报告数据应来自物理电路模型或明确的开关级器件模型。若只能使用平均模型，必须在报告中明确标注验证等级和限制。
