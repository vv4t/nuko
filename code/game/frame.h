/*
-- frame.h --

Shared data structures sent over the network. These are wrapped in 'frame'.
*/
#ifndef FRAME_H
#define FRAME_H

#include "bgame.h"
#include "../common/net.h"
#include "../common/log.h"

// The types of frames, what is happening in a frame -> 'netcmd'
typedef enum {
  NETCMD_CLIENT_INFO,
  NETCMD_USERCMD,
  NETCMD_SNAPSHOT,
  NETCMD_NAME,
  NETCMD_CHAT,
  NETCMD_SCORE
} netcmd_t;

// -- SERVER PACKETS --

// Information necessary for the client per round
// NOTE: this was originally intended to be sent only when a client initialised
// a connection. However, to send the name of the new map per round, I didn't
// want to create an entirely separate packet with only one variable. So these
// are combined. Generally, entity stays consistent per session
typedef struct {
  entity_t    entity;
  char        map_name[32];
} net_client_info_t;

// Snapshot of the game per frame
typedef struct {
  snapshot_t  d;
  int         seq; // The ID of the snapshot being sent
  int         ack; // The ID of the last usercmd sent by the client
} net_snapshot_t;

// -- CLIENT PAYLOADS --

// Usercmd of the client per frame
typedef struct {
  usercmd_t   d;
  int         ack; // The ID of the last snapshot sent by the server
} net_usercmd_t;

// Name change request
typedef struct {
  char        name[16];
} net_name_t;

// -- SHARED PACKETS --

// Chat message
typedef struct {
  char        content[256];
} net_chat_t;

// -------------------

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

// Read a frame from the socket and return the socket status
int   frame_read(sock_t sock, frame_t *frame);

// Send a frame through the socket
void  frame_send(sock_t sock, frame_t *frame);

#endif
