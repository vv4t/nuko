#include "net.h"

sock_t  sock_connect(const char *host);
void    sock_send(sock_t sock, void *payload, int len);
int     sock_recv(sock_t sock, void *payload, int max);

void    server_sock_listen(int port);
int     server_sock_accept(sock_t *sock);

#ifndef EMSCRIPTEN
  // TODO: linux implementation
#endif

void net_listen(int port)
{
  server_sock_listen(port);
}

bool net_accept(netchan_t *netchan)
{
  return server_sock_accept(&netchan->sock);
}

void netchan_connect(netchan_t *netchan, const char *host)
{
  netchan->sock = sock_connect(host);
  
  if (netchan->sock) {
    netchan->incoming_ack = 0;
    netchan->outgoing_ack = 0;
    
    netchan->incoming_seq = 0;
    netchan->outgoing_seq = 0;
  }
}

void netchan_send(netchan_t *netchan, void *payload, int len)
{
  sock_send(netchan->sock, payload, len);
}

int netchan_recv(netchan_t *netchan, void *payload, int max)
{
  return 0;
}
