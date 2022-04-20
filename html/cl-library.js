
function cl_get_host_address(host_ptr, max)
{
  const loc = window.location;
  const protocol = loc.protocol == "https:" ? "wss:" : "ws:";
  const host_address = protocol + "//" + loc.host + "/socket";
  
  const host_len = Math.min(host_address.length, max);
  
  for (let i = 0; i < host_len; i++)
    Module.HEAP8[host_ptr + i] = host_address.charCodeAt(i);
  
  Module.HEAP8[host_ptr + host_len] = 0;
}

function cl_net_connect(host_ptr)
{
  const host = read_heap_string(host_ptr);
  const ws = new WebSocket(host);
  ws.binaryType = "arraybuffer";
  
  const sock = new Module.socket_t((payload) => ws.send(payload));
  
  ws.onopen = () => sock.on_open();
  ws.onclose = () => sock.on_close();
  ws.onmessage = (e) => sock.on_recv(new Uint8Array(e.data));
  
  return Module.net_add_sock(sock);
}

function cl_net_sock_send(sock_id, payload_ptr, len)
{
  return Module.net_sock_send(sock_id, payload_ptr, len);
}

function cl_net_sock_recv(sock_id, payload_ptr, max)
{
  return Module.net_sock_recv(sock_id, payload_ptr, max);
}

mergeInto(LibraryManager.library, {
  cl_get_host_address: cl_get_host_address,
  net_connect: cl_net_connect,
  net_sock_send: cl_net_sock_send,
  net_sock_recv: cl_net_sock_recv
});
