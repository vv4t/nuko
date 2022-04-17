#include "client.h"

#include "../common/cmd.h"
#include <stdbool.h>

static void key_state(client_t *client, cl_button_t button, float state)
{
  client->buttons[button] = state;
}

static void in_forward_down(void *client) { key_state((client_t*) client, IN_FORWARD, 1.0f); }
static void in_forward_up(void *client)   { key_state((client_t*) client, IN_FORWARD, 0.0f); }
static void in_left_down(void *client)    { key_state((client_t*) client, IN_LEFT,    1.0f); }
static void in_left_up(void *client)      { key_state((client_t*) client, IN_LEFT,    0.0f); }
static void in_back_down(void *client)    { key_state((client_t*) client, IN_BACK,    1.0f); }
static void in_back_up(void *client)      { key_state((client_t*) client, IN_BACK,    0.0f); }
static void in_right_down(void *client)   { key_state((client_t*) client, IN_RIGHT,   1.0f); }
static void in_right_up(void *client)     { key_state((client_t*) client, IN_RIGHT,   0.0f); }
static void in_jump_down(void *client)    { key_state((client_t*) client, IN_JUMP,    1.0f); }
static void in_jump_up(void *client)      { key_state((client_t*) client, IN_JUMP,    0.0f); }

void client_input_init(client_t *client)
{
  cmd_add_command("+forward", in_forward_down, client);
  cmd_add_command("-forward", in_forward_up, client);
  cmd_add_command("+left", in_left_down, client);
  cmd_add_command("-left", in_left_up, client);
  cmd_add_command("+back", in_back_down, client);
  cmd_add_command("-back", in_back_up, client);
  cmd_add_command("+right", in_right_down, client);
  cmd_add_command("-right", in_right_up, client);
  cmd_add_command("+jump", in_jump_down, client);
  cmd_add_command("-jump", in_jump_up, client);
}

void client_mouse_move(client_t *client, int dx, int dy)
{
  client->mouse_x += dx;
  client->mouse_y += dy;
}

void client_base_move(client_t *client)
{
  float forward = client->buttons[IN_FORWARD];
  float left = client->buttons[IN_LEFT];
  float back = client->buttons[IN_BACK];
  float right = client->buttons[IN_RIGHT];
  float jump = client->buttons[IN_JUMP];
  
  client->usercmd.forward = forward - back;
  client->usercmd.right = right - left;
  client->usercmd.jump = jump;
  
  client->usercmd.yaw = client->mouse_x;
  client->usercmd.pitch = client->mouse_y;
}
