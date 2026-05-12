# SimAgent - Simulink AI 智能体

本项目是一个本地 Web 端 AI 智能体：浏览器前端通过 Node.js Bridge 连接 MathWorks Agentic Toolkit，再操作 MATLAB/Simulink。LLM 调用使用 OpenAI-compatible `/chat/completions` 接口，默认配置为 SiliconFlow。

## 架构

```text
Browser (index.html)
    -> HTTP / SSE (localhost:3000)
Node.js Bridge (server/bridge.js)
    -> stdio JSON-RPC 2.0
matlab-mcp-core-server.exe
    -> MATLAB / Simulink (existing session)
```

## 快速开始

### 1. 安装依赖

```bash
npm install
```

### 2. 启动 MATLAB

先打开 MATLAB，并确认 Simulink 可用。Bridge 默认使用 existing session 模式连接已经打开的 MATLAB。

### 3. 启动 Bridge Server

```bash
npm start
```

启动成功后会看到类似输出：

```text
========================================
SimAgent Bridge Server
URL: http://localhost:3000
MCP exe: C:/Users/XingTong/.matlab/agentic-toolkits/bin/matlab-mcp-core-server.exe
LLM API base: https://api.siliconflow.cn/v1
Default model: deepseek-ai/DeepSeek-V3
========================================
```

### 4. 打开前端

浏览器访问：

```text
http://localhost:3000
```

### 5. 配置 LLM 和 MCP

点击右上角的 MCP 状态按钮，填写：

- `LLM API Base`: `https://api.siliconflow.cn/v1`
- `LLM API Key`: 你的 SiliconFlow API Key
- `Default Model`: `deepseek-ai/DeepSeek-V3`
- `Bridge Server 地址`: `http://localhost:3000`

然后点击“连接 MCP”。配置会保存在浏览器 `localStorage` 中。

## 环境变量

也可以通过环境变量覆盖默认配置：

```powershell
$env:LLM_API_BASE="https://api.siliconflow.cn/v1"
$env:LLM_API_KEY="你的key"
$env:DEFAULT_MODEL="deepseek-ai/DeepSeek-V3"
npm start
```

如果前端没有传入 API Key，后端会尝试使用 `LLM_API_KEY`。

## 自定义 MCP 路径

如果你的 Agentic Toolkit 安装路径不同，可以改 [server/bridge.js](server/bridge.js) 顶部：

```js
const MCP_EXE = process.env.MCP_EXE
  || 'C:/Users/XingTong/.matlab/agentic-toolkits/bin/matlab-mcp-core-server.exe';

const MCP_ARGS = [
  '--matlab-session-mode=existing',
  '--extension-file=C:\\Users\\XingTong\\.matlab\\agentic-toolkits\\simulink\\tools\\tools.json'
];
```

或者用环境变量覆盖：

```powershell
$env:MCP_EXE="D:/custom/path/matlab-mcp-core-server.exe"
npm start
```

## API 接口

| 方法 | 路径 | 说明 |
| --- | --- | --- |
| GET | `/api/status` | 获取 MCP 状态、工具列表和默认 LLM 配置 |
| POST | `/api/connect` | 启动并连接 MCP 进程 |
| POST | `/api/disconnect` | 停止 MCP 进程 |
| POST | `/api/chat` | SSE 流式对话和工具调用 |
| GET | `/api/workspace?path=...` | 扫描工作区中的 `.slx`、`.m`、`.mat` 等文件 |

`POST /api/chat` 请求体示例：

```json
{
  "apiBase": "https://api.siliconflow.cn/v1",
  "apiKey": "你的key",
  "model": "deepseek-ai/DeepSeek-V3",
  "systemExtra": "",
  "contextFiles": [],
  "messages": [],
  "userMessage": "打开模型并运行仿真"
}
```

## 常见问题

### 端口 3000 被占用

如果启动时报错 `EADDRINUSE: address already in use :::3000`，说明已有进程占用了端口。可以换端口启动：

```powershell
$env:PORT=3001
npm start
```

然后访问：

```text
http://localhost:3001
```

也可以手动查找并关闭占用端口的进程：

```powershell
netstat -ano | Select-String ':3000'
Stop-Process -Id <PID> -Force
```

### 对话后发送按钮不可用

请确认已经拉取当前版本代码。后端会在 SSE 完成后关闭响应，前端也会在 `done` 事件后恢复按钮状态。

### MCP 连接失败

检查以下几项：

- MATLAB 已打开。
- Simulink 可正常使用。
- `MCP_EXE` 路径正确。
- `MCP_ARGS` 中的 `tools.json` 路径正确。

即使 MCP 失败，普通 LLM 对话仍会以 chat-only 模式运行。

## 开发模式

```bash
npm run dev
```

开发模式使用 Node.js watch，会在 [server/bridge.js](server/bridge.js) 变化后自动重启服务。
