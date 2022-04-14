#ifndef CLIENT_H
#define CLIENT_H

#include "../game/usercmd.h"

#define MAX_BUTTONS 5

typedef enum {
  IN_FORWARD,
  IN_LEFT,
  IN_BACK,
  IN_RIGHT,
  IN_JUMP
} cl_button_t;

typedef struct {
  usercmd_t   usercmd;
  
  cl_button_t buttons[MAX_BUTTONS];
  
  float       mouse_x;
  float       mouse_y;
} client_t;

void client_init(client_t *client);
void client_base_move(client_t *client);
void client_key_press(client_t *client, int key, int action);
void client_mouse_move(client_t *client, int dx, int dy);

#endif
