"use strict";

import fs from "fs";
import http from "http";
import express from "express";
import Module from "../server.cjs";
import { WebSocketServer } from "ws";

function http_init()
{
  const app = express();

  app.use(express.static("../../client/web"));

  const server = http.createServer(app);

  const wss = new WebSocketServer({
    server: server,
    path: "/socket"
  });

  wss.on("connection", function(ws) {
    const sock = new Module.socket_t((payload) => ws.send(payload));
    
    sock.on_open();
    ws.on("message", (payload) => sock.on_recv(payload));
    ws.on("close", () => sock.on_close());
    
    const sock_id = Module.net_add_sock(sock);
    
    console.log(`[${sock_id}] connected`);
    
    Module.net_serve(sock_id);
  });

  server.listen(8000);
  console.log("Listening on localhost:8000");
}

function game_init()
{
  Module.onRuntimeInitialised = function() {
    console.log(module);
  };
}

function main()
{
  http_init();
  game_init();
}

main();
