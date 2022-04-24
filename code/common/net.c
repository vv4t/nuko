#ifndef __EMSCRIPTEN__

#include "net.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define PORT 8000
#define MAX_CONNECTIONS 32

static int  net_sockets[MAX_CONNECTIONS];
static int  net_num_sockets = 0;

static int  net_server_fd;

static int net_add_sock()
{
  for (int i = 0; i < net_num_sockets; i++) {
    if (!net_sockets[i])
      return i;
  }
  
  return net_num_sockets++;
}

int net_connect(const char *host)
{
  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  
  if (inet_pton(AF_INET, host, &addr.sin_addr) != 1) {
    perror("inet_pton");
    return -1;
  }
  
  int conn_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (conn_fd == -1) {
    perror("socket");
    return -1;
  }
  
  if (connect(conn_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
    perror("connect");
    return -1;
  }
  
  fcntl(conn_fd, F_SETFL, O_NONBLOCK);
  
  int sock_id = net_add_sock();
  net_sockets[sock_id] = conn_fd;
  
  return sock_id;
}

void net_sock_send(int sock_id, void *buf, int len)
{
  if (!net_sockets[sock_id])
    return;
  
  send(net_sockets[sock_id], buf, len, MSG_NOSIGNAL);
}

int net_sock_read(int sock_id, void *buf, int len)
{
  if (!net_sockets[sock_id])
    return 0;
  
  int n = read(net_sockets[sock_id], buf, len);
  
  if (!n)
    net_sockets[sock_id] = 0;
  
  return n;
}

int net_listen()
{
  if ((net_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket");
    return 0;
  }
  
  int opt = 1;
  if (setsockopt(net_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    return 0;
  }
  
  fcntl(net_server_fd, F_SETFL, O_NONBLOCK);
  
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);
  
  if (bind(net_server_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
    perror("bind");
    return 0;
  }
  
  if (listen(net_server_fd, 3) == -1) {
    perror("listen");
    return 0;
  }
  
  return 1;
}

int net_accept()
{
  int sock_fd = accept(net_server_fd, NULL, 0);
  if (sock_fd > 0) {
    fcntl(sock_fd, F_SETFL, O_NONBLOCK);
    
    int sock_id = net_add_sock();
    net_sockets[sock_id] = sock_fd;
    
    return sock_id;
  } else {
    return -1;
  }
}

#endif
