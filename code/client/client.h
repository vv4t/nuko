#ifndef CLIENT_H
#define CLIENT_H

#include "cgame.h"
#include "../game/bgame.h"
#include "../common/net.h"
#include "../renderer/renderer.h"

#define MAX_USERCMDS 32

typedef struct {
  cgame_t     cg;
  renderer_t  renderer;
  int         sock_id;
  
  bool        connected;
  
  usercmd_t   usercmd;
  snapshot_t  snapshot;
  
  int         outgoing_seq;
  int         incoming_ack;
  usercmd_t   cmd_queue[MAX_USERCMDS];
} client_t;

void cl_init(client_t *cl);
void cl_load_map(client_t *client, const char *path);
void cl_update(client_t *cl);
void cl_predict(client_t *cl);

void cl_net_init(client_t *cl);
void cl_net_recv(client_t *cl);
void cl_send_cmd(client_t *cl);

void cl_input_init(client_t *cl);
void cl_base_move(client_t *cl);
void cl_mouse_move(client_t *cl, int dx, int dy);

#endif
