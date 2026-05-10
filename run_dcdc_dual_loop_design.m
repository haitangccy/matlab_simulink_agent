function run_dcdc_dual_loop_design
% Build and tune a dual-loop controlled switching Buck converter model.

clc;
close all;

outDir = fullfile(pwd, "results");
if ~exist(outDir, "dir")
    mkdir(outDir);
end

p = struct();
p.model = "dcdc_buck_dual_loop_switching";
p.Vin = 48;
p.Vref = 24;
p.Pout = 120;
p.R = p.Vref^2 / p.Pout;
p.fs = 50e3;
p.Ts = 1e-6;
p.Tstop = 0.04;
p.L = 220e-6;
p.C = 470e-6;
p.Vd = 0.7;
p.Dmin = 0.02;
p.Dmax = 0.95;
p.IrefMin = 0;
p.IrefMax = 12;
p.softStart = 0.008;
p.loadStepTime = 0.022;
p.loadStepFactor = 0.5;
p.inputStepTime = 0.030;
p.inputStepFactor = 0.9;
p.metricStart = 0.010;

fprintf("Creating Simulink model: %s\n", p.model);
createBuckModel(p);

fprintf("Tuning current inner loop...\n");
innerTable = tuneInnerLoop(p);
[~, idx] = min([innerTable.score]);
inner = innerTable(idx);
fprintf("Chosen inner loop: Kp_i=%g, Ki_i=%g\n", inner.Kp_i, inner.Ki_i);

fprintf("Tuning voltage outer loop with fixed inner loop...\n");
outerTable = tuneOuterLoop(p, inner);
[~, idx] = min([outerTable.score]);
best = outerTable(idx);
fprintf("Chosen dual-loop PI: Kp_i=%g, Ki_i=%g, Kp_v=%g, Ki_v=%g\n", ...
    best.Kp_i, best.Ki_i, best.Kp_v, best.Ki_v);

fprintf("Running switching-level verification with selected gains...\n");
wave = simulateSwitching(p, best);
metrics = computeMetrics(wave.t, wave.vo, wave.il, wave.d, p);

assignin("base", "Kp_i", best.Kp_i);
assignin("base", "Ki_i", best.Ki_i);
assignin("base", "Kp_v", best.Kp_v);
assignin("base", "Ki_v", best.Ki_v);
assignin("base", "Vin_nom", p.Vin);
assignin("base", "Vref", p.Vref);
assignin("base", "Rload", p.R);
assignin("base", "Lbuck", p.L);
assignin("base", "Cbuck", p.C);
assignin("base", "fsw", p.fs);
assignin("base", "Ts_ctrl", p.Ts);
mw = get_param(p.model, "ModelWorkspace");
assignin(mw, "Kp_i", best.Kp_i);
assignin(mw, "Ki_i", best.Ki_i);
assignin(mw, "Kp_v", best.Kp_v);
assignin(mw, "Ki_v", best.Ki_v);
set_param(p.model, "StopTime", num2str(p.Tstop));
save_system(p.model);

setMatlabFunctionCode(p.model + "/Dual_Loop_PI_PWM", controllerCode(p, best));
save_system(p.model);
fprintf("Running generated Simulink model once with SimulationInput...\n");
in = Simulink.SimulationInput(char(p.model));
in = in.setModelParameter("StopTime", num2str(p.Tstop));
in = in.setVariable("Kp_i", best.Kp_i);
in = in.setVariable("Ki_i", best.Ki_i);
in = in.setVariable("Kp_v", best.Kp_v);
in = in.setVariable("Ki_v", best.Ki_v);
out = sim(in);
simStatus = out.SimulationMetadata.ExecutionInfo.StopEvent;

save(fullfile(outDir, "dcdc_dual_loop_results.mat"), ...
    "p", "innerTable", "outerTable", "best", "wave", "metrics", "simStatus");
plotResults(wave, p, best, metrics, fullfile(outDir, "dcdc_dual_loop_waveforms.png"));
writeReport(p, innerTable, outerTable, best, metrics, simStatus, fullfile(outDir, "dcdc_dual_loop_design_report.md"));

fprintf("\nDone.\nModel: %s.slx\nReport: %s\nResults: %s\n", ...
    p.model, fullfile(outDir, "dcdc_dual_loop_design_report.md"), ...
    fullfile(outDir, "dcdc_dual_loop_results.mat"));
end

function createBuckModel(p)
if bdIsLoaded(p.model)
    close_system(char(p.model), 0);
end
if exist(p.model + ".slx", "file")
    delete(p.model + ".slx");
end

new_system(p.model);
open_system(p.model);
set_param(p.model, ...
    "SolverType", "Fixed-step", ...
    "Solver", "ode4", ...
    "FixedStep", num2str(p.Ts), ...
    "StopTime", num2str(p.Tstop), ...
    "SignalLogging", "on", ...
    "SignalLoggingName", "logsout");

add_block("simulink/Sources/Clock", p.model + "/Clock", "Position", [45 80 75 110]);
add_block("simulink/User-Defined Functions/MATLAB Function", p.model + "/Dual_Loop_PI_PWM", "Position", [155 50 335 150]);
add_block("simulink/User-Defined Functions/MATLAB Function", p.model + "/Buck_Switching_Plant", "Position", [430 55 630 150]);
add_block("simulink/Discrete/Unit Delay", p.model + "/Vo_feedback_delay", "Position", [665 50 700 80], "SampleTime", num2str(p.Ts), "InitialCondition", "0");
add_block("simulink/Discrete/Unit Delay", p.model + "/iL_feedback_delay", "Position", [665 95 700 125], "SampleTime", num2str(p.Ts), "InitialCondition", "0");
add_block("simulink/Sinks/Out1", p.model + "/Vo", "Position", [740 55 770 75]);
add_block("simulink/Sinks/Out1", p.model + "/iL", "Position", [740 95 770 115]);
add_block("simulink/Sinks/Out1", p.model + "/duty", "Position", [740 135 770 155]);
add_block("simulink/Sinks/Out1", p.model + "/iL_ref", "Position", [740 175 770 195]);

initGain = struct("Kp_i", 0.07, "Ki_i", 900, "Kp_v", 0.2, "Ki_v", 110);
ctrlCode = controllerCode(p, initGain);

plantFmt = strjoin([ ...
    "function [vo,il] = fcn(t,gate)\n" ...
    "persistent x_vo x_il\n" ...
    "vo = 0; il = 0;\n" ...
    "if isempty(x_vo)\n" ...
    "    x_vo = 0; x_il = 0;\n" ...
    "end\n" ...
    "Ts = %.17g; L = %.17g; C = %.17g; Vd = %.17g;\n" ...
    "Vin = %.17g;\n" ...
    "R = %.17g;\n" ...
    "if t >= %.17g\n" ...
    "    R = R * %.17g;\n" ...
    "end\n" ...
    "if t >= %.17g\n" ...
    "    Vin = Vin * %.17g;\n" ...
    "end\n" ...
    "if gate > 0.5\n" ...
    "    vL = Vin - x_vo;\n" ...
    "else\n" ...
    "    vL = -Vd - x_vo;\n" ...
    "end\n" ...
    "x_il = max(0, x_il + Ts * vL / L);\n" ...
    "x_vo = max(0, x_vo + Ts * (x_il - x_vo/R) / C);\n" ...
    "vo = x_vo; il = x_il;\n" ...
    "end\n"], "");
plantCode = sprintf(plantFmt, p.Ts, p.L, p.C, p.Vd, p.Vin, p.R, ...
    p.loadStepTime, p.loadStepFactor, p.inputStepTime, p.inputStepFactor);

setMatlabFunctionCode(p.model + "/Dual_Loop_PI_PWM", ctrlCode);
setMatlabFunctionCode(p.model + "/Buck_Switching_Plant", plantCode);

add_line(p.model, "Clock/1", "Dual_Loop_PI_PWM/1", "autorouting", "on");
add_line(p.model, "Buck_Switching_Plant/1", "Vo_feedback_delay/1", "autorouting", "on");
add_line(p.model, "Buck_Switching_Plant/2", "iL_feedback_delay/1", "autorouting", "on");
add_line(p.model, "Vo_feedback_delay/1", "Dual_Loop_PI_PWM/2", "autorouting", "on");
add_line(p.model, "iL_feedback_delay/1", "Dual_Loop_PI_PWM/3", "autorouting", "on");
add_line(p.model, "Clock/1", "Buck_Switching_Plant/1", "autorouting", "on");
add_line(p.model, "Dual_Loop_PI_PWM/1", "Buck_Switching_Plant/2", "autorouting", "on");
add_line(p.model, "Buck_Switching_Plant/1", "Vo/1", "autorouting", "on");
add_line(p.model, "Buck_Switching_Plant/2", "iL/1", "autorouting", "on");
add_line(p.model, "Dual_Loop_PI_PWM/2", "duty/1", "autorouting", "on");
add_line(p.model, "Dual_Loop_PI_PWM/3", "iL_ref/1", "autorouting", "on");

save_system(p.model);
end

function ctrlCode = controllerCode(p, g)
ctrlFmt = strjoin([ ...
    "function [gate,d,i_ref] = fcn(t,vo,il)\n" ...
    "persistent ev ei carrier\n" ...
    "gate = 0; d = 0; i_ref = 0;\n" ...
    "if isempty(ev)\n" ...
    "    ev = 0; ei = 0; carrier = 0;\n" ...
    "end\n" ...
    "Ts = %.17g; fs = %.17g;\n" ...
    "vref = %.17g * min(t/%.17g, 1);\n" ...
    "e_v = vref - vo;\n" ...
    "i_unsat = %.17g*e_v + ev;\n" ...
    "i_ref = min(max(i_unsat, %.17g), %.17g);\n" ...
    "if i_ref == i_unsat || sign(e_v) ~= sign(i_unsat - i_ref)\n" ...
    "    ev = ev + %.17g * e_v * Ts;\n" ...
    "end\n" ...
    "e_i = i_ref - il;\n" ...
    "d_unsat = %.17g*e_i + ei;\n" ...
    "d = min(max(d_unsat, %.17g), %.17g);\n" ...
    "if d == d_unsat || sign(e_i) ~= sign(d_unsat - d)\n" ...
    "    ei = ei + %.17g * e_i * Ts;\n" ...
    "end\n" ...
    "carrier = carrier + fs*Ts;\n" ...
    "if carrier >= 1\n" ...
    "    carrier = carrier - floor(carrier);\n" ...
    "end\n" ...
    "gate = double(carrier < d);\n" ...
    "end\n"], "");
ctrlCode = sprintf(ctrlFmt, p.Ts, p.fs, p.Vref, p.softStart, ...
    g.Kp_v, p.IrefMin, p.IrefMax, g.Ki_v, g.Kp_i, p.Dmin, p.Dmax, g.Ki_i);
end

function setMatlabFunctionCode(block, code)
rt = sfroot;
chart = rt.find("-isa", "Stateflow.EMChart", "Path", block);
chart.Script = code;
end

function rows = tuneInnerLoop(p)
kps = [0.018 0.025 0.035 0.05 0.07 0.10 0.14];
kis = [0 80 160 260 400 650 900];
rows = struct([]);
n = 0;
for a = 1:numel(kps)
    for b = 1:numel(kis)
        gain = struct("Kp_i", kps(a), "Ki_i", kis(b), "Kp_v", 0, "Ki_v", 0);
        wave = simulateCurrentLoop(p, gain);
        m = currentMetrics(wave.t, wave.il, wave.d);
        n = n + 1;
        rows(n).Kp_i = gain.Kp_i;
        rows(n).Ki_i = gain.Ki_i;
        rows(n).rise = m.rise;
        rows(n).tail_error = m.tail_error;
        rows(n).tail_std = m.tail_std;
        rows(n).duty_sat = m.duty_sat;
        rows(n).score = 4*m.tail_error + 0.8*m.rise + 0.4*m.tail_std + 2*m.duty_sat;
    end
end
end

function rows = tuneOuterLoop(p, inner)
kpv = [0.04 0.07 0.10 0.14 0.20 0.28 0.38];
kiv = [8 14 22 34 52 78 110];
rows = struct([]);
n = 0;
for a = 1:numel(kpv)
    for b = 1:numel(kiv)
        gain = struct("Kp_i", inner.Kp_i, "Ki_i", inner.Ki_i, "Kp_v", kpv(a), "Ki_v", kiv(b));
        wave = simulateAverage(p, gain);
        m = computeMetrics(wave.t, wave.vo, wave.il, wave.d, p);
        n = n + 1;
        rows(n).Kp_i = gain.Kp_i;
        rows(n).Ki_i = gain.Ki_i;
        rows(n).Kp_v = gain.Kp_v;
        rows(n).Ki_v = gain.Ki_v;
        rows(n).overshoot = m.overshoot;
        rows(n).settling_time = m.settling_time;
        rows(n).steady_error = m.steady_error;
        rows(n).tail_std = m.tail_std;
        rows(n).current_peak = m.current_peak;
        rows(n).duty_sat = m.duty_sat;
        rows(n).score = 6*m.steady_error + 3*m.overshoot + 1.5*m.settling_time + ...
            0.5*m.tail_std + 0.08*max(0, m.current_peak - p.IrefMax)^2 + 2*m.duty_sat;
    end
end
end

function wave = simulateCurrentLoop(p, g)
N = floor(0.015 / p.Ts) + 1;
t = (0:N-1)' * p.Ts;
vo = zeros(N,1);
il = zeros(N,1);
d = zeros(N,1);
ei = 0;
iref = zeros(N,1);
for k = 2:N
    iref(k) = 5 * (t(k) >= 0.001);
    e = iref(k) - il(k-1);
    u = g.Kp_i * e + ei;
    d(k) = min(max(u, p.Dmin), p.Dmax);
    if d(k) == u || sign(e) ~= sign(u - d(k))
        ei = ei + g.Ki_i * e * p.Ts;
    end
    vin = p.Vin;
    vl = d(k)*vin - vo(k-1) - (1-d(k))*p.Vd;
    il(k) = max(0, il(k-1) + p.Ts * vl / p.L);
    vo(k) = max(0, vo(k-1) + p.Ts * (il(k) - vo(k-1)/p.R) / p.C);
end
wave = struct("t", t, "vo", vo, "il", il, "d", d, "iref", iref);
end

function wave = simulateAverage(p, g)
N = floor(p.Tstop / p.Ts) + 1;
t = (0:N-1)' * p.Ts;
vo = zeros(N,1);
il = zeros(N,1);
d = zeros(N,1);
iref = zeros(N,1);
ev = 0; ei = 0;
for k = 2:N
    vref = p.Vref * min(t(k)/p.softStart, 1);
    vin = p.Vin * (1 - (1-p.inputStepFactor)*(t(k) >= p.inputStepTime));
    r = p.R * (1 - (1-p.loadStepFactor)*(t(k) >= p.loadStepTime));
    ev_err = vref - vo(k-1);
    i_unsat = g.Kp_v * ev_err + ev;
    iref(k) = min(max(i_unsat, p.IrefMin), p.IrefMax);
    if iref(k) == i_unsat || sign(ev_err) ~= sign(i_unsat - iref(k))
        ev = ev + g.Ki_v * ev_err * p.Ts;
    end
    ei_err = iref(k) - il(k-1);
    d_unsat = g.Kp_i * ei_err + ei;
    d(k) = min(max(d_unsat, p.Dmin), p.Dmax);
    if d(k) == d_unsat || sign(ei_err) ~= sign(d_unsat - d(k))
        ei = ei + g.Ki_i * ei_err * p.Ts;
    end
    vl = d(k)*vin - vo(k-1) - (1-d(k))*p.Vd;
    il(k) = max(0, il(k-1) + p.Ts * vl / p.L);
    vo(k) = max(0, vo(k-1) + p.Ts * (il(k) - vo(k-1)/r) / p.C);
end
wave = struct("t", t, "vo", vo, "il", il, "d", d, "iref", iref);
end

function wave = simulateSwitching(p, g)
N = floor(p.Tstop / p.Ts) + 1;
t = (0:N-1)' * p.Ts;
vo = zeros(N,1);
il = zeros(N,1);
d = zeros(N,1);
iref = zeros(N,1);
gate = zeros(N,1);
ev = 0; ei = 0; carrier = 0;
for k = 2:N
    vref = p.Vref * min(t(k)/p.softStart, 1);
    vin = p.Vin * (1 - (1-p.inputStepFactor)*(t(k) >= p.inputStepTime));
    r = p.R * (1 - (1-p.loadStepFactor)*(t(k) >= p.loadStepTime));
    ev_err = vref - vo(k-1);
    i_unsat = g.Kp_v * ev_err + ev;
    iref(k) = min(max(i_unsat, p.IrefMin), p.IrefMax);
    if iref(k) == i_unsat || sign(ev_err) ~= sign(i_unsat - iref(k))
        ev = ev + g.Ki_v * ev_err * p.Ts;
    end
    ei_err = iref(k) - il(k-1);
    d_unsat = g.Kp_i * ei_err + ei;
    d(k) = min(max(d_unsat, p.Dmin), p.Dmax);
    if d(k) == d_unsat || sign(ei_err) ~= sign(d_unsat - d(k))
        ei = ei + g.Ki_i * ei_err * p.Ts;
    end
    carrier = carrier + p.fs*p.Ts;
    if carrier >= 1
        carrier = carrier - floor(carrier);
    end
    gate(k) = carrier < d(k);
    if gate(k) > 0.5
        vl = vin - vo(k-1);
    else
        vl = -p.Vd - vo(k-1);
    end
    il(k) = max(0, il(k-1) + p.Ts * vl / p.L);
    vo(k) = max(0, vo(k-1) + p.Ts * (il(k) - vo(k-1)/r) / p.C);
end
wave = struct("t", t, "vo", vo, "il", il, "d", d, "iref", iref, "gate", gate);
end

function m = currentMetrics(t, il, d)
ref = 5;
tail = t > 0.010;
m.tail_error = abs(mean(il(tail)) - ref);
m.tail_std = std(il(tail));
idx = find(il > 0.9*ref, 1, "first");
if isempty(idx)
    m.rise = 1;
else
    m.rise = t(idx);
end
m.duty_sat = mean(d < 0.021 | d > 0.949);
end

function m = computeMetrics(t, vo, il, d, p)
tail = t > 0.035;
final = mean(vo(tail));
m.overshoot = max(0, (max(vo(t > p.metricStart)) - p.Vref) / p.Vref);
err = abs(vo - p.Vref) / p.Vref;
settleIdx = find(t > p.metricStart & movmax(err, max(1, round(0.002/p.Ts))) < 0.02, 1, "first");
if isempty(settleIdx)
    m.settling_time = p.Tstop;
else
    m.settling_time = t(settleIdx);
end
m.steady_error = abs(final - p.Vref) / p.Vref;
m.tail_std = std(vo(tail));
m.current_peak = max(il);
m.current_ripple = max(il(tail)) - min(il(tail));
m.duty_sat = mean(d < p.Dmin + 1e-6 | d > p.Dmax - 1e-6);
m.final_voltage = final;
end

function plotResults(w, p, best, m, fileName)
fig = figure("Visible", "off", "Color", "w", "Position", [100 100 1000 760]);
t = w.t * 1000;
subplot(4,1,1);
plot(t, w.vo, "LineWidth", 1.2); hold on; yline(p.Vref, "--");
grid on; ylabel("Vo (V)");
title(sprintf("Buck dual-loop PI: Kp_i=%.3g Ki_i=%.3g Kp_v=%.3g Ki_v=%.3g", ...
    best.Kp_i, best.Ki_i, best.Kp_v, best.Ki_v));
subplot(4,1,2);
plot(t, w.il, "LineWidth", 1.0); hold on; plot(t, w.iref, "--", "LineWidth", 0.9);
grid on; ylabel("iL (A)"); legend("iL", "iL ref", "Location", "best");
subplot(4,1,3);
plot(t, w.d, "LineWidth", 1.0);
grid on; ylabel("Duty");
subplot(4,1,4);
plot(t, w.gate, "LineWidth", 0.8);
grid on; ylabel("Gate"); xlabel("Time (ms)");
sgtitle(sprintf("Overshoot %.2f%%, settling %.2f ms, steady error %.2f%%, iL peak %.2f A", ...
    100*m.overshoot, 1000*m.settling_time, 100*m.steady_error, m.current_peak));
exportgraphics(fig, fileName, "Resolution", 160);
close(fig);
end

function writeReport(p, inner, outer, best, m, simStatus, fileName)
[~, iidx] = min([inner.score]);
[~, oidx] = min([outer.score]);
fid = fopen(fileName, "w", "n", "UTF-8");
cleanup = onCleanup(@() fclose(fid));
fprintf(fid, "# DC/DC Buck 开关电路与电压电流双闭环 PI 控制设计报告\n\n");
fprintf(fid, "生成日期：%s\n\n", string(datetime("now", "Format", "yyyy-MM-dd HH:mm:ss")));
fprintf(fid, "## 1. 设计目标与假设\n\n");
fprintf(fid, "- 拓扑：降压型 Buck DC/DC，PWM 开关级模型。\n");
fprintf(fid, "- 输入电压：%.1f V，输出目标：%.1f V，额定功率：%.1f W，等效负载：%.2f ohm。\n", p.Vin, p.Vref, p.Pout, p.R);
fprintf(fid, "- 开关频率：%.0f kHz，仿真步长：%.1f us。\n", p.fs/1000, p.Ts*1e6);
fprintf(fid, "- 功率级参数：L = %.0f uH，C = %.0f uF，二极管压降按 %.1f V 计。\n", p.L*1e6, p.C*1e6, p.Vd);
fprintf(fid, "- 扰动工况：%.1f ms 负载加重到 %.0f%%，%.1f ms 输入电压降到 %.0f%%。\n\n", ...
    p.loadStepTime*1000, p.loadStepFactor*100, p.inputStepTime*1000, p.inputStepFactor*100);

fprintf(fid, "## 2. 功率级与控制结构\n\n");
fprintf(fid, "模型文件为 `%s.slx`。模型包含 `Dual_Loop_PI_PWM` 和 `Buck_Switching_Plant` 两个核心子模块：前者实现电压外环、电流内环、限幅抗积分饱和与 PWM 载波比较；后者实现 Buck 开关导通/续流两种状态下的电感电流和输出电容电压更新。\n\n", p.model);
fprintf(fid, "控制链路为：`Vref -> 电压 PI -> iL_ref -> 电流 PI -> duty -> PWM -> Buck 功率级 -> Vo/iL`。电压环输出限幅为 %.1f 到 %.1f A，占空比限幅为 %.2f 到 %.2f。\n\n", ...
    p.IrefMin, p.IrefMax, p.Dmin, p.Dmax);

fprintf(fid, "## 3. PI 参数仿真整定过程\n\n");
fprintf(fid, "按照仓库技能中的顺序，先固定电压外环，只调电流内环；随后固定内环，再搜索电压外环。每组参数均计算超调、调节时间、稳态误差、尾段振荡、电流峰值和占空比限幅比例。\n\n");
fprintf(fid, "电流内环候选最优：`Kp_i = %.6g`，`Ki_i = %.6g`，内环评分 %.4g。\n\n", ...
    inner(iidx).Kp_i, inner(iidx).Ki_i, inner(iidx).score);
fprintf(fid, "电压外环候选最优：`Kp_v = %.6g`，`Ki_v = %.6g`，外环评分 %.4g。\n\n", ...
    outer(oidx).Kp_v, outer(oidx).Ki_v, outer(oidx).score);

fprintf(fid, "## 4. 当前已验证候选参数\n\n");
fprintf(fid, "| 参数 | 数值 | 状态 |\n");
fprintf(fid, "| --- | ---: | --- |\n");
fprintf(fid, "| Kp_i | %.6g | 在该模型下已验证 |\n", best.Kp_i);
fprintf(fid, "| Ki_i | %.6g | 在该模型下已验证 |\n", best.Ki_i);
fprintf(fid, "| Kp_v | %.6g | 在该模型下已验证 |\n", best.Kp_v);
fprintf(fid, "| Ki_v | %.6g | 在该模型下已验证 |\n\n", best.Ki_v);

fprintf(fid, "## 5. 开关级验证指标\n\n");
fprintf(fid, "| 指标 | 结果 |\n");
fprintf(fid, "| --- | ---: |\n");
fprintf(fid, "| 输出电压超调 | %.2f %% |\n", 100*m.overshoot);
fprintf(fid, "| 调节时间 | %.2f ms |\n", 1000*m.settling_time);
fprintf(fid, "| 稳态误差 | %.2f %% |\n", 100*m.steady_error);
fprintf(fid, "| 尾段电压标准差 | %.3f V |\n", m.tail_std);
fprintf(fid, "| 电感电流峰值 | %.3f A |\n", m.current_peak);
fprintf(fid, "| 稳态电感电流纹波 | %.3f A |\n", m.current_ripple);
fprintf(fid, "| 占空比限幅比例 | %.2f %% |\n\n", 100*m.duty_sat);

fprintf(fid, "## 6. 结论与使用说明\n\n");
fprintf(fid, "本轮结果属于“在该模型和上述工况下已验证”的候选参数。模型中包含软启动、负载阶跃、输入电压阶跃、占空比限幅和电流参考限幅，因此参数不是静态公式猜测，而是通过仿真指标搜索得到。\n\n");
fprintf(fid, "后续若要面向真实硬件，还需要加入 MOSFET 导通电阻、二极管反向恢复、ESR/ESL、采样延迟、ADC 量化、PWM 死区、驱动延迟和保护逻辑，再在输入电压/负载全范围内做边界验证。\n\n");
fprintf(fid, "波形图：`results/dcdc_dual_loop_waveforms.png`。仿真数据：`results/dcdc_dual_loop_results.mat`。\n");
fprintf(fid, "\n生成的 Simulink 模型已通过 `SimulationInput` 执行一次，停止事件：`%s`。\n", string(simStatus));
end
