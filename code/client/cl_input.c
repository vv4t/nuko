#include "client.h"

#include "../common/sys.h"
#include <stdbool.h>

#define KEY_FORWARD 'w'
#define KEY_LEFT    'a'
#define KEY_BACK    's'
#define KEY_RIGHT   'd'
#define KEY_JUMP    ' '

static float in_forward;
static float in_left;
static float in_back;
static float in_right;
static bool in_jump;

static float mouse_x = 0.0f, mouse_y = 0.0f;

void cl_key_press(int key, int action)
{
  switch (key) {
  case KEY_FORWARD:
    in_forward = (float) action;
    break;
  case KEY_LEFT:
    in_left = (float) action;
    break;
  case KEY_BACK:
    in_back = (float) action;
    break;
  case KEY_RIGHT:
    in_right = (float) action;
    break;
  case KEY_JUMP:
    in_jump = action;
    break;
  }
}

void cl_mouse_move(int dx, int dy)
{
  mouse_x += dx;
  mouse_y += dy;
}

void cl_base_move(usercmd_t *usercmd)
{
  usercmd->forward = in_forward - in_back;
  usercmd->right = in_right - in_left;
  usercmd->yaw = mouse_x;
  usercmd->pitch = mouse_y;
  usercmd->jump = in_jump;
}
