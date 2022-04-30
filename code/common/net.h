#ifndef NET_H
#define NET_H

typedef int sock_t;

sock_t  net_connect(const char *host);
void    net_sock_send(sock_t sock, void *buf, int len);
int     net_sock_read(sock_t sock, void *buf, int len);

int     net_listen();
sock_t  net_accept();

#endif
