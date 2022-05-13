#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "bgame.h"
#include "../common/net.h"
#include "../common/log.h"

typedef enum {
  NETCMD_CLIENT_INFO,
  NETCMD_USERCMD,
  NETCMD_SNAPSHOT,
  NETCMD_NAME,
  NETCMD_CHAT,
  NETCMD_SCORE
} netcmd_t;

typedef struct {
  entity_t    entity;
  char        map_name[32];
} net_client_info_t;

typedef struct {
  char        name[16];
} net_name_t;

typedef struct {
  char        content[256];
} net_chat_t;

typedef struct {
  usercmd_t   d;
  int         ack;
} net_usercmd_t;

typedef struct {
  snapshot_t  d;
  int         seq;
  int         ack;
} net_snapshot_t;

typedef union {
  net_client_info_t client_info;
  net_name_t        name;
  net_chat_t        chat;
  net_usercmd_t     usercmd;
  net_snapshot_t    snapshot;
} net_data_t;

typedef struct {
  netcmd_t    netcmd;
  net_data_t  data;
} frame_t;

int   frame_read(sock_t sock, frame_t *frame);
void  frame_send(sock_t sock, frame_t *frame);

#endif
