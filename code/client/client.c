#include "client.h"

#include "../common/sys.h"
#include <stdbool.h>

#define KEY_FORWARD 'w'
#define KEY_LEFT    'a'
#define KEY_BACK    's'
#define KEY_RIGHT   'd'
#define KEY_JUMP    ' '

void client_init(client_t *client)
{
  *client = (client_t) { 0 };
}

void client_key_press(client_t *client, int key, int action)
{
  switch (key) {
  case KEY_FORWARD:
    client->in_forward = (float) action;
    break;
  case KEY_LEFT:
    client->in_left = (float) action;
    break;
  case KEY_BACK:
    client->in_back = (float) action;
    break;
  case KEY_RIGHT:
    client->in_right = (float) action;
    break;
  case KEY_JUMP:
    client->in_jump = action;
    break;
  }
}

void client_mouse_move(client_t *client, int dx, int dy)
{
  client->mouse_x += dx;
  client->mouse_y += dy;
}

void client_base_move(client_t *client)
{
  client->usercmd.forward = client->in_forward - client->in_back;
  client->usercmd.right = client->in_right - client->in_left;
  client->usercmd.jump = client->in_jump;
  
  client->usercmd.yaw = client->mouse_x;
  client->usercmd.pitch = client->mouse_y;
}
