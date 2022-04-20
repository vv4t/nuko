#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "bgame.h"

typedef enum {
  NETCMD_OPEN,
  NETCMD_USERCMD,
  NETCMD_SNAPSHOT
} netcmd_t;

typedef struct {
  netcmd_t  netcmd;
  int       outgoing_seq;
  int       outgoing_ack;
  union {
    entity_t    client_entity;
    usercmd_t   usercmd;
    snapshot_t  snapshot;
  } data;
} frame_t;

#endif
