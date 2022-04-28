#include "client.h"

#include "../common/cmd.h"
#include <stdbool.h>

static float  in_forward  = 0.0f;
static float  in_left     = 0.0f;
static float  in_back     = 0.0f;
static float  in_right    = 0.0f;
static float  in_jump     = 0.0f;

static float  in_mouse_x = 0.0f;
static float  in_mouse_y = 0.0f;

static void in_forward_down(void *cl) { in_forward  = 1.0f; }
static void in_forward_up(void *cl)   { in_forward  = 0.0f; }
static void in_left_down(void *cl)    { in_left     = 1.0f; }
static void in_left_up(void *cl)      { in_left     = 0.0f; }
static void in_back_down(void *cl)    { in_back     = 1.0f; }
static void in_back_up(void *cl)      { in_back     = 0.0f; }
static void in_right_down(void *cl)   { in_right    = 1.0f; }
static void in_right_up(void *cl)     { in_right    = 0.0f; }
static void in_jump_down(void *cl)    { in_jump     = 1.0f; }
static void in_jump_up(void *cl)      { in_jump     = 0.0f; }

void cl_input_init(client_t *cl)
{
  cmd_add_command("+forward", in_forward_down, NULL);
  cmd_add_command("-forward", in_forward_up, NULL);
  cmd_add_command("+left", in_left_down, NULL);
  cmd_add_command("-left", in_left_up, NULL);
  cmd_add_command("+back", in_back_down, NULL);
  cmd_add_command("-back", in_back_up, NULL);
  cmd_add_command("+right", in_right_down, NULL);
  cmd_add_command("-right", in_right_up, NULL);
  cmd_add_command("+jump", in_jump_down, NULL);
  cmd_add_command("-jump", in_jump_up, NULL);
}

void cl_mouse_move(client_t *cl, int dx, int dy)
{
  in_mouse_x += dx;
  in_mouse_y += dy;
}

void cl_base_move(client_t *cl)
{
  cl->usercmd.forward = in_forward - in_back;
  cl->usercmd.right   = in_right - in_left;
  cl->usercmd.jump    = in_jump;
  cl->usercmd.yaw     = in_mouse_x;
  cl->usercmd.pitch   = in_mouse_y;
}
