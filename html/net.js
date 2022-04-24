"use strict";

const NET_SOCK_DISCONNECTED = 0;
const NET_SOCK_CONNECTED = 1;

Module.socket_t = function(fn_send)
{
  this.b_recv = [];
  this.fn_send = fn_send;
  this.connected = false;
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
  this.connected = true;
};

Module.socket_t.prototype.on_recv = function(payload)
{
  this.b_recv.push(payload);
};

Module.socket_t.prototype.on_close = function()
{
  this.connected = false;
};

Module.net_sockets = {};
Module.net_incoming_sockets = [];
Module.net_sock_uid = 0;

Module.net_add_sock = function(sock)
{
  const uid = Module.net_sock_uid++;
  Module.net_sockets[uid] = sock;
  
  return uid;
}

Module.net_sock_send = function(sock_id, payload_ptr, len)
{
  const sock = Module.net_sockets[sock_id];
  
  if (!sock)
    return;
  
  if (sock.connected) {
    const payload = new Uint8Array(len);
    
    for (let i = 0; i < len; i++)
      payload[i] = Module.HEAP8[payload_ptr + i];
    
    sock.send(payload);
  }
}

Module.net_sock_recv = function(sock_id, payload_ptr, max)
{
  const sock = Module.net_sockets[sock_id];
  
  if (!sock)
    return;
  
  const payload = sock.recv();
  if (payload) {
    const payload_len = Math.min(max, payload.byteLength);
    for (let i = 0; i < payload_len; i++)
      Module.HEAP8[payload_ptr + i] = payload[i];
    
    return payload_len;
  }
  
  return 0;
}

Module.net_sock_status = function(sock_id)
{
  const sock = Module.net_sockets[sock_id];
  
  if (!sock)
    return NET_SOCK_DISCONNECTED;
  
  if (sock.connected) {
    return NET_SOCK_CONNECTED;
  } else {
    delete Module.net_sockets[sock_id];
    return NET_SOCK_DISCONNECTED;
  }
}

Module.net_listen = function(port)
{
  console.log("net_listen(): unimplemented");
}

Module.net_serve = function(sock_id)
{
  Module.net_incoming_sockets.push(sock_id);
}

Module.net_accept = function(sock_ptr)
{
  const sock_id = Module.net_incoming_sockets.shift();
  if (sock_id != undefined) {
    Module.HEAP32[Math.floor(sock_ptr / 4)] = sock_id;
    return true;
  }
  
  return false;
}

