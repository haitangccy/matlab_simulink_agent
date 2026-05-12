function optimize_buck48to12_physical_full_report()
%OPTIMIZE_BUCK48TO12_PHYSICAL_FULL_REPORT Comprehensive physical-model PI tuning.
% Tunes both single voltage-loop PI and cascaded voltage-current PI on the
% same physical SPS buck circuit, then regenerates data, plots, and PDF report.

rootDir = fileparts(mfilename("fullpath"));
outDir = fullfile(rootDir, "buck48to12_physical_outputs");
if ~exist(outDir, "dir"), mkdir(outDir); end

mdl = "buck48to12_physical_power_circuit";
if ~isfile(fullfile(rootDir, mdl + ".slx"))
    build_buck48to12_physical_slx();
end
load_system(mdl);

cfg = struct("Vref",12,"tStop",8e-3,"vBand",0.02);

fprintf("Comprehensive physical-model PI optimization on %s.slx\n", mdl);

% Single-loop refinement. The previous coarse best was near Kp=0.012, Ki=22,
% but other stable points existed at higher proportional gain, so scan a wider
% local map instead of only one narrow box.
singleKp = [0.006 0.009 0.012 0.016 0.022 0.030 0.045 0.065];
singleKi = [14 18 22 27 33 42 55 75];

singleRecords = [];
iter = 0;
totalSingle = numel(singleKp) * numel(singleKi);
for a = 1:numel(singleKp)
    for b = 1:numel(singleKi)
        iter = iter + 1;
        gains = struct("ctrl_mode",1,"Kp_v",singleKp(a),"Ki_v",singleKi(b),"Kp_i",0,"Ki_i",0);
        [met,~] = runCandidate(mdl, cfg, gains);
        singleRecords = [singleRecords; makeRecord("single_voltage_loop",iter,gains,met)]; %#ok<AGROW>
        fprintf("single %02d/%02d Kp=%.4g Ki=%.4g score=%.3f\n", iter,totalSingle,gains.Kp_v,gains.Ki_v,met.score);
    end
end
singleTable = sortrows(struct2table(singleRecords), "score");
bestSingle = singleTable(1,:);

% Dual-loop comprehensive result: use the two-stage refined search if present;
% otherwise perform the stage-2 search now.
stage2File = fullfile(outDir, "buck48to12_physical_second_refine.mat");
if ~isfile(stage2File)
    refine_buck48to12_physical_pi_stage2();
end
loaded = load(stage2File, "secondRefineTable", "bestSecond");
dualTable = loaded.secondRefineTable;
dualTable.method = repmat("dual_voltage_current_loop", height(dualTable), 1);
bestDual = loaded.bestSecond;
bestDual.method = "dual_voltage_current_loop";

[metricsSingle,waveSingle] = runCandidate(mdl, cfg, rowToGains(bestSingle));
[metricsDual,waveDual] = runCandidate(mdl, cfg, rowToGains(bestDual));

comparison = table( ...
    ["single_voltage_loop"; "dual_voltage_current_loop"], ...
    [bestSingle.Kp_v; bestDual.Kp_v], ...
    [bestSingle.Ki_v; bestDual.Ki_v], ...
    [nan; bestDual.Kp_i], ...
    [nan; bestDual.Ki_i], ...
    [metricsSingle.overshoot_pct; metricsDual.overshoot_pct], ...
    [metricsSingle.settling_ms; metricsDual.settling_ms], ...
    [metricsSingle.steady_error_V; metricsDual.steady_error_V], ...
    [metricsSingle.v_ripple_mV; metricsDual.v_ripple_mV], ...
    [metricsSingle.iL_peak_A; metricsDual.iL_peak_A], ...
    [metricsSingle.duty_sat_pct; metricsDual.duty_sat_pct], ...
    [metricsSingle.score; metricsDual.score], ...
    'VariableNames', {'method','Kp_v','Ki_v','Kp_i','Ki_i','overshoot_pct','settling_ms', ...
    'steady_error_V','v_ripple_mV','iL_peak_A','duty_sat_pct','score'});

if metricsDual.score < metricsSingle.score
    bestMethod = "dual_voltage_current_loop";
else
    bestMethod = "single_voltage_loop";
end

figWave = plotWaves(waveSingle, waveDual, outDir);
figTune = plotTune(singleTable, dualTable, outDir);
figBest = plotBestBars(comparison, outDir);

save(fullfile(outDir, "buck48to12_physical_final_optimization.mat"), ...
    "singleTable","dualTable","bestSingle","bestDual","comparison", ...
    "metricsSingle","metricsDual","waveSingle","waveDual","bestMethod");

createReport(outDir, comparison, singleTable, dualTable, bestMethod, figWave, figTune, figBest);

fprintf("\nFinal comprehensive comparison:\n");
disp(comparison);
fprintf("Updated final report: %s\n", fullfile(outDir, "buck48to12_physical_experiment_report.pdf"));
end

function [met,w] = runCandidate(mdl, cfg, gains)
in = Simulink.SimulationInput(mdl);
in = in.setModelParameter("StopTime", num2str(cfg.tStop));
names = fieldnames(gains);
for k = 1:numel(names)
    in = in.setVariable(names{k}, gains.(names{k}));
end
try
    out = sim(in);
    w = extractWave(out);
    met = calcMetrics(cfg, w);
catch ME
    warning(ME.identifier, "%s", ME.message);
    w = failedWave();
    met = failedMetrics();
end
end

function w = extractWave(out)
w.t = out.Vo_phys.time(:);
w.Vo = out.Vo_phys.signals.values(:);
w.tIL = out.iL_phys.time(:);
w.iL = out.iL_phys.signals.values(:);
w.tDuty = out.duty_phys.time(:);
w.duty = out.duty_phys.signals.values(:);
w.iL_on_v_grid = interp1(w.tIL, w.iL, w.t, "linear", "extrap");
w.duty_on_v_grid = interp1(w.tDuty, w.duty, w.t, "previous", "extrap");
end

function met = calcMetrics(cfg,w)
t = w.t;
Vo = w.Vo;
iL = w.iL_on_v_grid;
duty = w.duty_on_v_grid;
tail = t > (t(end)-1e-3);
evalMask = t > 1.2e-3;
met.final_V = Vo(end);
met.overshoot_pct = max(0, (max(Vo(evalMask)) - cfg.Vref)/cfg.Vref*100);
outside = abs(Vo - cfg.Vref) > cfg.vBand*cfg.Vref;
lastOutside = find(outside & evalMask, 1, "last");
if isempty(lastOutside)
    met.settling_ms = 1.2;
else
    met.settling_ms = t(lastOutside)*1e3;
end
met.steady_error_V = abs(mean(Vo(tail)) - cfg.Vref);
met.v_ripple_mV = (max(Vo(tail)) - min(Vo(tail))) * 1000;
met.iL_peak_A = max(abs(iL));
met.duty_sat_pct = mean(duty < 0.021 | duty > 0.919) * 100;
bad = any(~isfinite(Vo)) || max(abs(Vo)) > 80 || met.iL_peak_A > 80;
met.score = 1e5*bad + met.settling_ms + 4*met.overshoot_pct + 80*met.steady_error_V + ...
    0.01*met.v_ripple_mV + 0.5*max(0,met.iL_peak_A-25) + 0.2*met.duty_sat_pct;
end

function met = failedMetrics()
met = struct("final_V",nan,"overshoot_pct",999,"settling_ms",999,"steady_error_V",999, ...
    "v_ripple_mV",999,"iL_peak_A",999,"duty_sat_pct",100,"score",1e6);
end

function w = failedWave()
w = struct("t",0,"Vo",nan,"tIL",0,"iL",nan,"tDuty",0,"duty",nan, ...
    "iL_on_v_grid",nan,"duty_on_v_grid",nan);
end

function rec = makeRecord(method,iter,gains,met)
rec = gains;
rec.method = string(method);
rec.iter = iter;
names = fieldnames(met);
for k = 1:numel(names)
    rec.(names{k}) = met.(names{k});
end
end

function gains = rowToGains(row)
gains = struct("ctrl_mode",row.ctrl_mode,"Kp_v",row.Kp_v,"Ki_v",row.Ki_v, ...
    "Kp_i",row.Kp_i,"Ki_i",row.Ki_i);
end

function figFile = plotWaves(ws,wd,outDir)
fig = figure("Color","w","Position",[100 100 1150 820]);
tiledlayout(3,1,"TileSpacing","compact");
nexttile;
plot(ws.t*1e3, ws.Vo, "LineWidth",1.1); hold on;
plot(wd.t*1e3, wd.Vo, "LineWidth",1.1);
yline(12,"--"); grid on; ylabel("Vo / V");
legend("optimized single loop","optimized dual loop","Vref","Location","southeast");
title("Physical SPS Buck optimized output voltage");
nexttile;
plot(ws.tIL*1e3, ws.iL, "LineWidth",1.1); hold on;
plot(wd.tIL*1e3, wd.iL, "LineWidth",1.1);
grid on; ylabel("iL / A"); legend("single","dual","Location","southeast");
nexttile;
plot(ws.tDuty*1e3, ws.duty, "LineWidth",1.1); hold on;
plot(wd.tDuty*1e3, wd.duty, "LineWidth",1.1);
grid on; ylabel("Duty"); xlabel("Time / ms"); legend("single","dual","Location","southeast");
figFile = fullfile(outDir, "physical_waveforms.png");
exportgraphics(fig, figFile, "Resolution", 220);
close(fig);
end

function figFile = plotTune(singleTable,dualTable,outDir)
fig = figure("Color","w","Position",[100 100 1120 490]);
tiledlayout(1,2,"TileSpacing","compact");
nexttile;
scatter(singleTable.overshoot_pct, singleTable.steady_error_V, 46, singleTable.score, "filled");
grid on; xlabel("Overshoot / %"); ylabel("Steady error / V");
title("Optimized single-loop candidates"); colorbar;
nexttile;
scatter(dualTable.overshoot_pct, dualTable.steady_error_V, 46, dualTable.score, "filled");
grid on; xlabel("Overshoot / %"); ylabel("Steady error / V");
title("Optimized dual-loop candidates"); colorbar;
figFile = fullfile(outDir, "physical_tuning_map.png");
exportgraphics(fig, figFile, "Resolution", 220);
close(fig);
end

function figFile = plotBestBars(comparison,outDir)
fig = figure("Color","w","Position",[100 100 1080 520]);
tiledlayout(1,3,"TileSpacing","compact");
labels = categorical(comparison.method);
nexttile; bar(labels, comparison.score); grid on; ylabel("Score"); title("Overall score");
nexttile; bar(labels, comparison.overshoot_pct); grid on; ylabel("%"); title("Overshoot");
nexttile; bar(labels, comparison.v_ripple_mV); grid on; ylabel("mV"); title("Output ripple");
figFile = fullfile(outDir, "physical_final_metric_bars.png");
exportgraphics(fig, figFile, "Resolution", 220);
close(fig);
end

function createReport(outDir, comparison, singleTable, dualTable, bestMethod, figWave, figTune, figBest)
import mlreportgen.report.*
import mlreportgen.dom.*
rpt = Report(fullfile(outDir, "buck48to12_physical_experiment_report"), "pdf");
rpt.Locale = "zh_CN";
add(rpt, TitlePage("Title","48V 转 12V Buck 物理电路模型 PI 全面优化报告", ...
    "Subtitle","单电压环与电压-电流双环均在同一 SPS 物理电路上调参", ...
    "Author","MATLAB MCP"));
add(rpt, TableOfContents);

ch = Chapter("Title","模型与优化范围");
add(ch, Paragraph(['本报告基于 buck48to12_physical_power_circuit.slx。功率级使用 Specialized Power Systems 物理器件连接，' ...
    '包括 48V 直流源、高边 MOSFET、续流二极管、电感、电容 ESR、负载、电压/电流测量、Electrical Ground 与 powergui。' ...
    '控制器只生成 PWM 门极信号，不用函数模型替代功率级。']));
add(ch, Paragraph(sprintf("单环本轮扫描 %d 组候选参数；双环采用二次细化后的 %d 组候选记录，并重新复核最佳波形。", ...
    height(singleTable), height(dualTable))));
add(rpt,ch);

ch = Chapter("Title","最终对比");
add(ch, MATLABTable(comparison));
add(ch, Paragraph(char("综合评分越低越好；当前推荐方案：" + bestMethod)));
add(rpt,ch);

ch = Chapter("Title","图片结果");
img = Image(figWave); img.Style = {ScaleToFit(true)}; add(ch,img);
img = Image(figTune); img.Style = {ScaleToFit(true)}; add(ch,img);
img = Image(figBest); img.Style = {ScaleToFit(true)}; add(ch,img);
add(rpt,ch);

ch = Chapter("Title","候选参数记录");
add(ch, Paragraph("单环评分前 10 组："));
add(ch, MATLABTable(singleTable(1:min(10,height(singleTable)),:)));
add(ch, Paragraph("双环评分前 10 组："));
add(ch, MATLABTable(dualTable(1:min(10,height(dualTable)),:)));
add(rpt,ch);

close(rpt);
end
