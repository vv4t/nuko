#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "bgame.h"

typedef enum {
  NETCMD_CLIENT_INFO,
  NETCMD_USERCMD,
  NETCMD_SNAPSHOT
} netcmd_t;

typedef struct {
  netcmd_t  netcmd;
  union {
    struct {
      entity_t    entity;
    } client_info;
    struct {
      int         ack;
      usercmd_t   d;
    } usercmd;
    struct {
      int         seq;
      int         ack;
      snapshot_t  d;
    } snapshot;
  } data;
} frame_t;

#endif
