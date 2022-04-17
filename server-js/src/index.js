"use strict";

import http from "http";
import express from "express";
import { WebSocketServer } from "ws";

const app = express();

app.use(express.static("../bin/html/client"));

const server = http.createServer(app);

const wss = new WebSocketServer({
  server: server,
  path: "/socket"
});

wss.on("connection", function(ws) {
  ws.on("message", function(message) {
    console.log("received: %s", message);
  });
  
  ws.send("hi");
});

server.listen(8000);
