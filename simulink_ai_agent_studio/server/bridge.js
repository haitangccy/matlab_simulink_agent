/**
 * SimAgent MCP Bridge Server
 *
 * Responsibilities:
 *  1. Start matlab-mcp-core-server.exe as a child process (stdio transport)
 *  2. Implement MCP JSON-RPC 2.0 over stdio
 *  3. Expose HTTP APIs for the frontend
 *  4. Run an OpenAI-compatible agentic tool loop
 */

const express = require('express');
const cors    = require('cors');
const path    = require('path');
const { spawn } = require('child_process');

// Config

const MCP_EXE  = process.env.MCP_EXE
  || 'C:/Users/XingTong/.matlab/agentic-toolkits/bin/matlab-mcp-core-server.exe';

const MCP_ARGS = [
  '--matlab-session-mode=existing',
  '--extension-file=C:\\Users\\XingTong\\.matlab\\agentic-toolkits\\simulink\\tools\\tools.json'
];

const PORT = process.env.PORT || 3000;
const DEFAULT_LLM_API_BASE = process.env.LLM_API_BASE || 'https://api.siliconflow.cn/v1';
const DEFAULT_MODEL = process.env.DEFAULT_MODEL || 'deepseek-ai/DeepSeek-V3';

// MCP Process Manager

class McpProcess {
  constructor() {
    this.proc       = null;
    this.ready      = false;
    this.tools      = [];     // cached tool list from initialize
    this._pendingMap = new Map(); // id -> { resolve, reject }
    this._buffer    = '';
    this._nextId    = 1;
  }

  start() {
    return new Promise((resolve, reject) => {
      console.log('[MCP] Spawning:', MCP_EXE);
      this.proc = spawn(MCP_EXE, MCP_ARGS, {
        stdio: ['pipe', 'pipe', 'pipe'],
        windowsHide: true
      });

      this.proc.stdout.setEncoding('utf8');
      this.proc.stdout.on('data', chunk => this._onData(chunk));

      this.proc.stderr.setEncoding('utf8');
      this.proc.stderr.on('data', d => console.error('[MCP stderr]', d.trim()));

      this.proc.on('error', err => {
        console.error('[MCP] Process error:', err.message);
        reject(err);
      });

      this.proc.on('exit', (code) => {
        console.warn('[MCP] Process exited with code', code);
        this.ready = false;
      });

      // Initialize handshake
      this._initialize().then(result => {
        console.log('[MCP] Initialized, server:', result?.serverInfo?.name);
        return this._listTools();
      }).then(tools => {
        this.tools = tools;
        this.ready = true;
        console.log(`[MCP] ${tools.length} tools available:`, tools.map(t => t.name).join(', '));
        resolve(tools);
      }).catch(reject);
    });
  }

  stop() {
    if (this.proc) { this.proc.kill(); this.proc = null; }
    this.ready = false;
  }

  /** Low-level: send JSON-RPC request, return promise of result */
  _request(method, params = {}) {
    return new Promise((resolve, reject) => {
      const id = this._nextId++;
      const msg = JSON.stringify({ jsonrpc: '2.0', id, method, params });
      this._pendingMap.set(id, { resolve, reject });
      this.proc.stdin.write(msg + '\n');

      // timeout
      setTimeout(() => {
        if (this._pendingMap.has(id)) {
          this._pendingMap.delete(id);
          reject(new Error(`MCP request timeout: ${method}`));
        }
      }, 60_000);
    });
  }

  /** Notification (no id, no response expected) */
  _notify(method, params = {}) {
    const msg = JSON.stringify({ jsonrpc: '2.0', method, params });
    this.proc.stdin.write(msg + '\n');
  }

  _onData(chunk) {
    this._buffer += chunk;
    const lines = this._buffer.split('\n');
    this._buffer = lines.pop(); // incomplete last line
    for (const line of lines) {
      const trimmed = line.trim();
      if (!trimmed) continue;
      try {
        const msg = JSON.parse(trimmed);
        if (msg.id !== undefined && this._pendingMap.has(msg.id)) {
          const { resolve, reject } = this._pendingMap.get(msg.id);
          this._pendingMap.delete(msg.id);
          if (msg.error) reject(new Error(msg.error.message || JSON.stringify(msg.error)));
          else resolve(msg.result);
        }
      } catch (e) {
        console.warn('[MCP] Non-JSON line:', trimmed.slice(0, 120));
      }
    }
  }

  async _initialize() {
    return this._request('initialize', {
      protocolVersion: '2024-11-05',
      capabilities: { tools: {} },
      clientInfo: { name: 'SimAgent', version: '1.0.0' }
    });
  }

  async _listTools() {
    const res = await this._request('tools/list', {});
    return res?.tools || [];
  }

  /** Call a single MCP tool */
  async callTool(name, args) {
    if (!this.ready) throw new Error('MCP not ready');
    const res = await this._request('tools/call', { name, arguments: args });
    // MCP returns { content: [ {type, text} ] }
    if (res?.content) {
      return res.content.map(c => c.text || JSON.stringify(c)).join('\n');
    }
    return JSON.stringify(res);
  }
}

// Global MCP instance

const mcp = new McpProcess();
let mcpStartPromise = null;

async function ensureMcp() {
  if (mcp.ready) return;
  if (!mcpStartPromise) {
    mcpStartPromise = mcp.start().catch(err => {
      mcpStartPromise = null;
      throw err;
    });
  }
  return mcpStartPromise;
}

// Agentic Loop

/**
 * Run one agentic turn:
 *   - call an OpenAI-compatible chat completion endpoint with tools enabled
 *   - if the model requests tool calls, dispatch to MCP, feed results back
 *   - loop until the model returns a final text reply
 *
 * @param {string} apiKey
 * @param {string} model
 * @param {string} systemPrompt
 * @param {Array}  history  - [{role, content}]
 * @param {Function} onEvent - SSE callback (type, data)
 */
async function agenticLoop(apiKey, apiBase, model, systemPrompt, history, onEvent) {

  // Build tool definitions from MCP tool list
  const toolDefs = mcp.tools.map(t => ({
    type: 'function',
    function: {
      name: t.name,
      description: t.description || t.name,
      parameters: t.inputSchema || { type: 'object', properties: {} }
    }
  }));

  const base = (apiBase || DEFAULT_LLM_API_BASE).replace(/\/+$/, '');
  const endpoint = `${base}/chat/completions`;
  const selectedModel = model || DEFAULT_MODEL;
  let messages = [
    { role: 'system', content: systemPrompt },
    ...history
  ];

  // eslint-disable-next-line no-constant-condition
  while (true) {
    onEvent('status', { text: `Calling model ${selectedModel}...` });

    const params = {
      model: selectedModel,
      max_tokens: 4096,
      messages,
    };
    if (toolDefs.length > 0) params.tools = toolDefs;

    const httpRes = await fetch(endpoint, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${apiKey}`
      },
      body: JSON.stringify(params)
    });

    if (!httpRes.ok) {
      const body = await httpRes.text();
      throw new Error(`LLM HTTP ${httpRes.status}: ${body.slice(0, 1000)}`);
    }

    const response = await httpRes.json();
    const message = response.choices?.[0]?.message;
    if (!message) {
      throw new Error(`Invalid LLM response: ${JSON.stringify(response).slice(0, 1000)}`);
    }

    if (message.content) onEvent('text', { text: message.content });

    const toolCalls = message.tool_calls || [];

    if (toolCalls.length === 0) {
      // Done
      onEvent('done', {});
      return;
    }

    // Add assistant turn to history
    messages.push(message);

    // Execute tool calls
    for (const toolCall of toolCalls) {
      const name = toolCall.function?.name;
      let input = {};
      try {
        input = toolCall.function?.arguments ? JSON.parse(toolCall.function.arguments) : {};
      } catch {
        input = { rawArguments: toolCall.function?.arguments || '' };
      }
      onEvent('tool_call', { name, input });

      let resultText;
      try {
        resultText = await mcp.callTool(name, input);
        onEvent('tool_result', { name, result: resultText });
      } catch (err) {
        resultText = `ERROR: ${err.message}`;
        onEvent('tool_error', { name, error: err.message });
      }

      messages.push({
        role: 'tool',
        tool_call_id: toolCall.id,
        name,
        content: resultText
      });
    }
  }
}

// Express App

const app = express();
app.use(cors());
app.use(express.json({ limit: '4mb' }));
app.use(express.static(path.join(__dirname, '../public')));
app.use(express.static(path.join(__dirname, '..')));

/** GET /api/status - MCP health */
app.get('/api/status', (req, res) => {
  res.json({
    mcpReady: mcp.ready,
    tools: mcp.tools.map(t => ({ name: t.name, description: t.description })),
    pid: mcp.proc?.pid || null,
    llm: {
      apiBase: DEFAULT_LLM_API_BASE,
      defaultModel: DEFAULT_MODEL
    }
  });
});

/** POST /api/connect - start MCP process */
app.post('/api/connect', async (req, res) => {
  try {
    await ensureMcp();
    res.json({ ok: true, tools: mcp.tools.map(t => t.name) });
  } catch (err) {
    res.status(500).json({ ok: false, error: err.message });
  }
});

/** POST /api/disconnect */
app.post('/api/disconnect', (req, res) => {
  mcp.stop();
  mcpStartPromise = null;
  res.json({ ok: true });
});

/**
 * POST /api/chat (SSE stream)
 *
 * Body: { apiBase, apiKey, model, systemExtra, messages: [{role,content}], userMessage }
 *
 * Streams events:
 *   data: {"type":"status","text":"..."}
 *   data: {"type":"text","text":"..."}
 *   data: {"type":"tool_call","name":"...","input":{}}
 *   data: {"type":"tool_result","name":"...","result":"..."}
 *   data: {"type":"tool_error","name":"...","error":"..."}
 *   data: {"type":"done"}
 *   data: {"type":"error","error":"..."}
 */
app.post('/api/chat', async (req, res) => {
  const { apiBase, apiKey, model, systemExtra, messages, userMessage, contextFiles, fileContents } = req.body;

  const effectiveApiKey = apiKey || process.env.LLM_API_KEY;
  const effectiveApiBase = apiBase || DEFAULT_LLM_API_BASE;
  const effectiveModel = model || DEFAULT_MODEL;

  if (!effectiveApiKey) return res.status(400).json({ error: 'apiKey required' });

  // SSE headers
  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');
  res.flushHeaders();

  const send = (type, data) => {
    res.write(`data: ${JSON.stringify({ type, ...data })}\n\n`);
  };

  try {
    // Try to ensure MCP (non-fatal if unavailable)
    try { await ensureMcp(); } catch (e) {
      send('status', { text: `MCP is not connected (${e.message}); running in chat-only mode.` });
    }

    // Build file context block from pre-read file contents
    let fileContextBlock = '';
    if (fileContents && fileContents.length > 0) {
      const blocks = fileContents.map(f => {
        if (f.content) {
          return `### 文件：${f.name} (${f.path})\n\`\`\`\n${f.content}\n\`\`\`${f.truncated ? `\n*(仅显示前 200 行，共 ${f.totalLines} 行)*` : ''}`;
        } else {
          return `### 文件：${f.name} (${f.path})\n*(${f.note || '二进制文件，无法读取文本内容'})*`;
        }
      });
      fileContextBlock = `\n\n## 用户已选中的工作区文件内容：\n${blocks.join('\n\n')}`;
    } else if (contextFiles?.length) {
      // fallback: just mention file names (old behaviour)
      fileContextBlock = `\nUser has selected these workspace files: ${contextFiles.join(', ')}`;
    }

    const systemPrompt = [
      'You are SimAgent, an expert AI assistant specialized in MATLAB and Simulink.',
      'You help users build, configure, analyze, and debug Simulink models.',
      'When MCP tools are available, use them to perform real operations on MATLAB/Simulink.',
      'Always reply in the same language as the user.',
      fileContextBlock,
      systemExtra || ''
    ].filter(Boolean).join('\n');

    const history = [
      ...(messages || []).map(m => ({ role: m.role, content: m.content })),
      { role: 'user', content: userMessage }
    ];

    await agenticLoop(effectiveApiKey, effectiveApiBase, effectiveModel, systemPrompt, history, send);
    res.end();
  } catch (err) {
    send('error', { error: err.message });
    res.end();
  }
});

/** GET /api/workspace?path=... - list .slx .m .mat files */
app.get('/api/workspace', (req, res) => {
  const fs = require('fs');
  const dirPath = req.query.path || process.cwd();
  try {
    const entries = fs.readdirSync(dirPath, { withFileTypes: true });
    const files = entries
      .filter(e => e.isFile() && /\.(slx|m|mat|mlx|mdl)$/i.test(e.name))
      .map(e => ({ name: e.name, path: path.join(dirPath, e.name) }));
    res.json({ ok: true, files });
  } catch (err) {
    res.json({ ok: false, error: err.message, files: [] });
  }
});

/**
 * POST /api/readfile
 * Body: { filePath: "C:/workspace/pid.m" }
 * Returns: { ok, name, path, size, encoding, content }
 *
 * Readable text files (.m .mlx .mat metadata): returns UTF-8 text content.
 * Binary files (.slx .mdl .mat): returns a summary instead of raw bytes.
 */
app.post('/api/readfile', (req, res) => {
  const fs = require('fs');
  const { filePath } = req.body;
  if (!filePath) return res.status(400).json({ ok: false, error: 'filePath required' });

  // Security: only allow whitelisted extensions
  const allowed = /\.(m|mlx|mat|slx|mdl|txt|csv|json|xml|log)$/i;
  if (!allowed.test(filePath)) {
    return res.status(403).json({ ok: false, error: 'File type not allowed' });
  }

  try {
    const stat = fs.statSync(filePath);
    const sizeMB = stat.size / 1024 / 1024;
    const name = path.basename(filePath);
    const ext = path.extname(filePath).toLowerCase();

    // Text-readable extensions
    const textExts = ['.m', '.mlx', '.txt', '.csv', '.json', '.xml', '.log'];
    if (textExts.includes(ext)) {
      if (sizeMB > 2) {
        // Too large: read first 200 lines only
        const raw = fs.readFileSync(filePath, 'utf-8');
        const lines = raw.split('\n');
        const preview = lines.slice(0, 200).join('\n');
        return res.json({
          ok: true, name, path: filePath,
          size: stat.size,
          encoding: 'utf-8',
          truncated: true,
          totalLines: lines.length,
          content: preview,
          note: `文件较大 (${sizeMB.toFixed(1)} MB)，仅显示前 200 行`
        });
      }
      const content = fs.readFileSync(filePath, 'utf-8');
      return res.json({ ok: true, name, path: filePath, size: stat.size, encoding: 'utf-8', truncated: false, content });
    }

    // Binary files (.slx .mdl .mat): can't read as text, return metadata summary
    return res.json({
      ok: true, name, path: filePath, size: stat.size,
      encoding: 'binary',
      content: null,
      note: `${ext.toUpperCase()} 是二进制文件，无法直接读取文本内容。文件路径已传给 AI，请通过 MCP 工具操作该文件。`
    });

  } catch (err) {
    res.status(500).json({ ok: false, error: err.message });
  }
});

// Start

app.listen(PORT, () => {
  console.log(`
========================================
SimAgent Bridge Server
URL: http://localhost:${PORT}
MCP exe: ${MCP_EXE}
LLM API base: ${DEFAULT_LLM_API_BASE}
Default model: ${DEFAULT_MODEL}
========================================
`);
});

process.on('exit', () => mcp.stop());
process.on('SIGINT', () => { mcp.stop(); process.exit(0); });
