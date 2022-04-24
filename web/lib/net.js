"use strict";

const SOCK_CONNECTING = 0;
const SOCK_CONNECTED = 1;
const SOCK_DISCONNECTED = 2;

Module.socket_t = function(fn_send)
{
  this.b_recv = [];
  this.fn_send = fn_send;
  this.status = SOCK_CONNECTING;
};

Module.socket_t.prototype.send = function(payload)
{
  this.fn_send(payload);
};

Module.socket_t.prototype.recv = function()
{
  return this.b_recv.shift();
};

Module.socket_t.prototype.on_open = function()
{
  this.status = SOCK_CONNECTED;
};

Module.socket_t.prototype.on_recv = function(payload)
{
  this.b_recv.push(payload);
};

Module.socket_t.prototype.on_close = function()
{
  this.status = SOCK_DISCONNECTED;
};

Module.net_sockets = [];
Module.net_incoming_sockets = [];

Module.net_add_sock = function(sock)
{
  for (let i = 0; i < Module.net_sockets.length; i++) {
    if (!Module.net_sockets[i]) {
      Module.net_sockets[i] = sock;
      return i;
    }
  }
  
  return Module.net_sockets.push(sock) - 1;
}

Module.net_sock_send = function(sock_id, payload_ptr, len)
{
  const sock = Module.net_sockets[sock_id];
  
  if (!sock)
    return;
  
  if (sock.status == SOCK_CONNECTED) {
    const payload = new Uint8Array(len);
    
    for (let i = 0; i < len; i++)
      payload[i] = Module.HEAP8[payload_ptr + i];
    
    sock.send(payload);
  }
}

Module.net_sock_read = function(sock_id, payload_ptr, len)
{
  const sock = Module.net_sockets[sock_id];
  
  if (!sock)
    return 0;
  
  if (sock.status == SOCK_DISCONNECTED) {
    Module.net_sockets[sock_id] = null;
    return 0;
  }
  
  const payload = sock.recv();
  if (payload) {
    const payload_len = Math.min(len, payload.byteLength);
    
    for (let i = 0; i < payload_len; i++)
      Module.HEAP8[payload_ptr + i] = payload[i];
    
    return payload_len;
  }
  
  return -1;
}


Module.net_listen = function()
{
  console.log("net_listen(): unimplemented");
}

Module.net_serve = function(sock_id)
{
  Module.net_incoming_sockets.push(sock_id);
}

Module.net_accept = function()
{
  const sock_id = Module.net_incoming_sockets.shift();
  
  if (sock_id != undefined)
    return sock_id;
  
  return -1;
}
