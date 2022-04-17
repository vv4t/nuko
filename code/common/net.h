#ifndef NET_H
#define NET_H

#include <stdbool.h>

typedef int sock_t;

typedef struct {
  sock_t  sock;
  
  int     incoming_ack;
  int     incoming_seq;
  
  int     outgoing_ack;
  int     outgoing_seq;
} netchan_t;

void  net_listen(int port);
bool  net_accept(netchan_t *netchan);

void  netchan_connect(netchan_t *netchan, const char *host);
void  netchan_send(netchan_t *netchan, void *payload, int len);
int   netchan_recv(netchan_t *netchan, void *payload, int max);

#endif
