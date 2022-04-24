#ifndef CLIENT_H
#define CLIENT_H

#include "cgame.h"
#include "../game/bgame.h"
#include "../common/net.h"
#include "../renderer/renderer.h"

#define MAX_BUTTONS 5

typedef enum {
  IN_FORWARD,
  IN_LEFT,
  IN_BACK,
  IN_RIGHT,
  IN_JUMP
} cl_button_t;

typedef struct {
  cgame_t     cg;
  renderer_t  renderer;
  int         sock_id;
  
  usercmd_t   usercmd;
  
  cl_button_t buttons[MAX_BUTTONS];
  
  float       mouse_x;
  float       mouse_y;
} client_t;

void cl_init(client_t *client);
void cl_net_init(client_t *client);
void cl_poll(client_t *client);
void cl_input_init(client_t *client);
void cl_load_map(client_t *client, const char *path);
void cl_update(client_t *client);
void cl_base_move(client_t *client);
void cl_mouse_move(client_t *client, int dx, int dy);
void cl_send_cmd(client_t *client);

#endif
