function emc_sock_new()
{
  
}

function emc_sock_connect(host_ptr)
{
  const host = read_heap_string(host_ptr);
  const sid = Module.sockets.push(new socket_t(host)) - 1;
  return sid;
}

function emc_sock_send(sid, payload_ptr, len)
{
  const ws = Module.sockets[sid];
  const payload = new Uint8Array(len);
  
  for (let i = 0; i < len; i++)
    payload[i] = Module.HEAP8[payload_ptr + i];
  
  if (ws.ws.readyState == 1)
    ws.ws.send(payload);
}

function emc_sock_recv(sid, payload_ptr, len)
{
  
}

mergeInto(LibraryManager.library, {
  sock_new: emc_sock_new,
  sock_connect: emc_sock_connect,
  sock_send: emc_sock_send,
  sock_recv: emc_sock_recv
});
