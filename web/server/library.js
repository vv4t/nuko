function sv_net_connect(host_ptr)
{
  console.log("net_connect(): unimplemented");
}

function sv_net_sock_send(sock_id, payload_ptr, len)
{
  return Module.net_sock_send(sock_id, payload_ptr, len);
}

function sv_net_sock_read(sock_id, payload_ptr, max)
{
  return Module.net_sock_read(sock_id, payload_ptr, max);
}

function sv_net_listen()
{
  console.log("net_listen(): unimplemented");
}

function sv_net_accept()
{
  return Module.net_accept();
}

mergeInto(LibraryManager.library, {
  net_connect: sv_net_connect,
  net_sock_send: sv_net_sock_send,
  net_sock_read: sv_net_sock_read,
  net_listen: sv_net_listen,
  net_accept: sv_net_accept
});
