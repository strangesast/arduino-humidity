const fs = require("fs");
const WebSocket = require("ws");
const http = require("http");

const vals = [];
const log_path = process.env.LOG_PATH || "log.txt";
if (!fs.lstatSync(log_path).isFile()) {
  throw new Error("invalid log file.");
}

async function* lines(s) {
  let previous = "";
  for await (const chunk of s) {
    previous += chunk;
    let i;
    while ((i = previous.indexOf("\n")) >= 0) {
      const line = previous.slice(0, i + 1);
      yield line;
      previous = previous.slice(i + 1);
    }
  }
  if (previous.length > 0) {
    yield previous;
  }
}

const server = http.createServer((request, response) => {
  let body = "";
  request.on("data", (data) => (body += data));
  request.on("end", async () => {
    response.writeHead(200, { "Content-Type": "text/plain" });
    response.end();

    let value;
    try {
      value = JSON.parse(body);
    } catch (err) {
      console.error(`invalid json: "${body}"`);
      return;
    }

    const line = JSON.stringify({ ts: Date.now(), value }) + "\n";
    console.log(`line ${line}`);
    wss.clients.forEach((ws) => {
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(line);
      }
    });
    fs.appendFile(log_path, line, (err) =>
      err ? console.error("failed to write to file") : null
    );
  });
});

const wss = new WebSocket.Server({ server });
wss.on("connection", async (ws) => {
  console.log("client connected.");
  const stream = fs.createReadStream(log_path);
  for await (const line of lines(stream)) {
    ws.send(line);
  }
});

const port = process.env.PORT || 3000;
const host = "0.0.0.0";

server.listen(port, host, () =>
  console.log(`server listening on ${host}:${port}`)
);
