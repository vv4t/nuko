#ifndef CL_LOCAL_H
#define CL_LOCAL_H

#include "client.h"

#include "input.h"
#include "renderer.h"
#include "../game/protocol.h"
#include "../game/map_file.h"
#include "../common/net.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX_CMD_QUEUE   128

typedef struct {
  sock_t      sock;
  bool        connected;
  
  int         snapshot_ack;
  snapshot_t  snapshot;
  
  usercmd_t   usercmd;
  
  int         cmd_tail;
  int         cmd_head;
  usercmd_t   cmd_queue[MAX_CMD_QUEUE];
} client_t;

extern client_t cl;

// cl_cgame.c
void  cl_snapshot();
void  cl_interpolate(float interp);
void  cl_view_look();
void  cl_predict();
void  cl_reconcile();
void  cl_load_map(const char *path);

// cl_net.c
void  cl_connect(const char *host);
void  cl_parse();
void  cl_parse_snapshot(const frame_t *frame);
void  cl_parse_client_info(const frame_t *frame);
void  cl_parse_chat(const frame_t *frame);
void  cl_send_cmd();
void  cl_send_chat(const char *text);

#endif
