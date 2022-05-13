"use strict";

const SOCK_CONNECTING = 0;
const SOCK_CONNECTED = 1;
const SOCK_DISCONNECTED = 2;

Module.socket_t = function(fn_send, fn_disconnect)
{
  this.b_recv = [];
  this.b_ptr = 0;
  this.fn_send = fn_send;
  this.fn_disconnect = fn_disconnect;
  this.status = SOCK_CONNECTING;
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

Module.net_sock_disconnect = function(sock_id)
{
  if (!Module.net_sockets[sock_id])
    return;
  
  Module.net_sockets[sock_id].fn_disconnect();
  Module.net_sockets[sock_id] = null;
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
    
    sock.fn_send(payload);
  }
}

Module.net_sock_read = function(sock_id, payload_ptr, len)
{
  const sock = Module.net_sockets[sock_id];
  
  if (!sock)
    return 0;
  
  if (sock.status == SOCK_DISCONNECTED) {
    Module.net_sock_disconnect(sock_id);
    return 0;
  }
  
  if (sock.b_recv.length == 0)
    return -1;
  
  for (let i = 0; i < len; i++) {
    Module.HEAP8[payload_ptr + i] = sock.b_recv[0][sock.b_ptr];
    
    if (++sock.b_ptr >= sock.b_recv[0].length) {
      if (!sock.b_recv.shift())
        return i + 1;
      else
        sock.b_ptr = 0;
    }
  }
  
  return len;
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
