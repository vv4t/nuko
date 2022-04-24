#ifndef NET_H
#define NET_H

int   net_connect(const char *host);
void  net_sock_send(int sock_id, void *buf, int len);
int   net_sock_read(int sock_id, void *buf, int len);

int   net_listen();
int   net_accept();

#endif
