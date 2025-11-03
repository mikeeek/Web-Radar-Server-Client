const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
const basicAuth = require('express-basic-auth');

const isPkg = typeof process.pkg !== 'undefined';
const baseDir = isPkg ? path.dirname(process.execPath) : __dirname;

const app = express();
const server = http.createServer(app);

// --- WebSocket server
const wss = new WebSocket.Server({ server, path: "/cs2_webradar" });

function randomTitle(length = 12) {
  const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  let title = "";
  for (let i = 0; i < length; i++) title += chars.charAt(Math.floor(Math.random() * chars.length));
  return title;
}
const randomWindowTitle = randomTitle();
process.title = randomWindowTitle;
console.log(`\x1b]2;${randomWindowTitle}\x1b\x5c`);

const PORT = 22006;
const HOST = '0.0.0.0';

// --- Basic Auth
app.use(basicAuth({
  users: { 'radar': 'password' }, 
  challenge: true,
  unauthorizedResponse: () => 'Access denied',
}));

// --- Body parser for JSON
app.use(express.json({ limit: '1mb' }));

let latestData = "{}";

// --- WebSocket logic
wss.on('connection', (ws) => {
  console.log("Client connected");
  ws.send(latestData); // send latest immediately

  broadcastClientCount();


  ws.on('close', () => console.log("Client disconnected"));
});

function broadcastClientCount() {
  const count = wss.clients.size;
  const payload = JSON.stringify({ type: 'client_count', count });
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(payload);
    }
  });
}

// --- HTTP API: accept JSON from C++ via POST 
app.post('/update', (req, res) => {
  latestData = JSON.stringify(req.body);
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) client.send(latestData);
  });
  res.sendStatus(200);
});

// =====================
// Serve React build
// =====================
// 1) Static files from the React build
const distPath = path.join(baseDir, 'client', 'dist');
app.use(express.static(distPath));
app.get('*', (req, res) => {
  res.sendFile(path.join(distPath, 'index.html'));
});

// 2) SPA fallback: send index.html for any non-API/WS route
//    (keeps /update working; WebSocket upgrades bypass this)
app.get('*', (req, res) => {
  res.sendFile(path.join(distPath, 'index.html'));
});

// --- Start server
server.listen(PORT, HOST, () => {
  console.log(`Radar server listening on http://${HOST}:${PORT}`);
  console.log(`WebSocket path ws://<host>:${PORT}/cs2_webradar`);
});
