#ifndef CLIENT_H
#define CLIENT_H

#include "../game/usercmd.h"
#include "../renderer/renderer.h"
#include "../common/net.h"
#include "../cgame/cgame.h"

#define MAX_BUTTONS 5

typedef enum {
  IN_FORWARD,
  IN_LEFT,
  IN_BACK,
  IN_RIGHT,
  IN_JUMP
} cl_button_t;

typedef struct {
  cgame_t     cgame;
  renderer_t  renderer;
  netchan_t   netchan;
  
  usercmd_t   usercmd;
  
  cl_button_t buttons[MAX_BUTTONS];
  
  float       mouse_x;
  float       mouse_y;
} client_t;

void client_init(client_t *client);
void client_net_init(client_t *client);
void client_input_init(client_t *client);
void client_load_map(client_t *client, const char *path);
void client_update(client_t *client);
void client_base_move(client_t *client);
void client_mouse_move(client_t *client, int dx, int dy);
void client_send_cmd(client_t *client);

#endif
