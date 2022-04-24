#ifndef NET_H
#define NET_H

#include <stdbool.h>

typedef enum {
  NET_SOCK_DISCONNECTED,
  NET_SOCK_CONNECTED
} sock_status_t;

typedef int sock_t;

sock_t        net_connect(const char *host);
void          net_sock_send(sock_t sock, void *payload, int len);
int           net_sock_recv(sock_t sock, void *payload, int max);
sock_status_t net_sock_status(sock_t sock);

void          net_listen(int port);
int           net_accept(sock_t *sock);

#endif
