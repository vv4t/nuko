#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "bgame.h"

typedef enum {
  NETCMD_CLIENT_INFO,
  NETCMD_USERCMD,
  NETCMD_SNAPSHOT,
  NETCMD_NAME,
  NETCMD_CHAT,
  NETCMD_SCORE
} netcmd_t;

typedef struct {
  netcmd_t  netcmd;
  union {
    struct {
      entity_t    entity;
      char        map_name[32];
    } client_info;
    struct {
      char        name[16];
    } name;
    struct {
      char        content[256];
    } chat;
    struct {
      usercmd_t   d;
      int         ack;
    } usercmd;
    struct {
      snapshot_t  d;
      int         seq;
      int         ack;
    } snapshot;
  } data;
} frame_t;

#endif
