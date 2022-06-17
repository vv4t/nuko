/*
-- net.h --

Networking based functions.
NOTE: platform dependent implementation.
*/

#ifndef NET_H
#define NET_H

typedef int sock_t;

// Connect to a server
sock_t  net_connect(const char *host);

// Send data through the socket
void    net_sock_send(sock_t sock, void *buf, int len);

// Read data from the socket
int     net_sock_read(sock_t sock, void *buf, int len);

// Disconnect a socket
void    net_sock_disconnect(sock_t sock);

// Begin listen for clients
int     net_listen();

// Accept incoming clients
sock_t  net_accept();

#endif
