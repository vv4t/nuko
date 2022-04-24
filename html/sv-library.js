function sv_net_connect(host_ptr)
{
  console.log("net_connect(): unimplemented");
}

function sv_net_sock_send(sock_id, payload_ptr, len)
{
  return Module.net_sock_send(sock_id, payload_ptr, len);
}

function sv_net_sock_recv(sock_id, payload_ptr, max)
{
  return Module.net_sock_recv(sock_id, payload_ptr, max);
}

function sv_net_sock_status(sock_id)
{
  return Module.net_sock_status(sock_id);
}

function sv_net_listen(port)
{
  console.log("net_listen(): unimplemented");
}

function sv_net_accept(sock_ptr)
{
  return Module.net_accept(sock_ptr);
}

mergeInto(LibraryManager.library, {
  net_connect: sv_net_connect,
  net_sock_send: sv_net_sock_send,
  net_sock_recv: sv_net_sock_recv,
  net_sock_status: sv_net_sock_status,
  net_listen: sv_net_listen,
  net_accept: sv_net_accept
});
