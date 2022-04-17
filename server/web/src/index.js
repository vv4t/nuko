"use strict";

import fs from "fs";
import http from "http";
import express from "express";
import Module from "../server.cjs";
import { WebSocketServer } from "ws";

class ws_client_t {
  constructor(ws)
  {
    this.ws = ws;
  }
};

Module.sv_clients = [];
Module.sv_clients_tail = 0;
Module.sv_clients_head = 0;

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
    const id = Module.sv_clients_head++;
    Module.sv_clients[id] = new ws_client_t(ws);
    
    console.log(`[${id}] connected`);
    
    ws.on("message", function(message) {
      const payload = Buffer.from(message);
      console.log(`[${id}] received:`, message, payload);
    });
    
    ws.on("close", function() {
      console.log(`[${id}] disconnected`);
    });
    
    ws.send("hi");
  });

  server.listen(8000);
  console.log("Listening on localhosts:8000");
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
