#ifndef CL_LOCAL_H
#define CL_LOCAL_H

#include "cgame.h"
#include "renderer.h"
#include "../game/bgame.h"
#include "../common/net.h"
#include "../game/protocol.h"

#define MAX_CMD_QUEUE 512

typedef struct {
  cgame_t     cg;
  sock_t      sock;
  
  bool        connected;
  
  snapshot_t  snapshot;
  
  int         cmd_tail;
  int         cmd_head;
  usercmd_t   cmd_queue[MAX_CMD_QUEUE];
} client_t;

extern client_t cl;

void cl_fixed_update();
void cl_load_map(const char *path);
void cl_predict();
void cl_connect(const char *host);
void cl_parse();
void cl_parse_snapshot(const frame_t *frame);
void cl_parse_client_info(const frame_t *frame);
void cl_send_cmd();

#endif
