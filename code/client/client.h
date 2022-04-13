#ifndef CLIENT_H
#define CLIENT_H

#include "../common/usercmd.h"

typedef struct {
  usercmd_t   usercmd;
  
  float       in_forward;
  float       in_left;
  float       in_back;
  float       in_right;
  float       in_jump;
  
  float       mouse_x;
  float       mouse_y;
} client_t;

void client_init(client_t *client);
void client_base_move(client_t *client);
void client_key_press(client_t *client, int key, int action);
void client_mouse_move(client_t *client, int dx, int dy);

#endif
