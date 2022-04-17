
function emc_server_sock_listen(port)
{
  // ignore
}

function emc_server_sock_accept(sock_ptr)
{
  if (Module.sv_clients_tail < Module.sv_clients_head) {
    const sock = Module.sv_clients_tail++;
    Module.HEAP32[Math.floor(sock_ptr / 4)] = sock;
    
    return true;
  }
  
  return false;
}

mergeInto(LibraryManager.library, {
  server_sock_listen: emc_server_sock_listen,
  server_sock_accept: emc_server_sock_accept
});
