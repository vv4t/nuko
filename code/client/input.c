#include "input.h"

#define MAX_PITCH     1.57
#define MAX_KEYBINDS  32

typedef struct {
  int         key;
  const char  *text;
} keybind_t;

static keybind_t    in_keybinds[MAX_KEYBINDS];
static int          in_num_keybinds = 0;

static float  in_forward;
static float  in_left;
static float  in_back;
static float  in_right;
static float  in_jump;
static float  in_attack;

static float  in_pitch;
static float  in_yaw;

static void in_forward_down(void *d) { in_forward = 1.0f; }
static void in_forward_up(void *d)   { in_forward = 0.0f; }
static void in_left_down(void *d)    { in_left    = 1.0f; }
static void in_left_up(void *d)      { in_left    = 0.0f; }
static void in_back_down(void *d)    { in_back    = 1.0f; }
static void in_back_up(void *d)      { in_back    = 0.0f; }
static void in_right_down(void *d)   { in_right   = 1.0f; }
static void in_right_up(void *d)     { in_right   = 0.0f; }
static void in_jump_down(void *d)    { in_jump    = 1.0f; }
static void in_jump_up(void *d)      { in_jump    = 0.0f; }
static void in_attack_down(void *d)  { in_attack  = 1.0f;  }
static void in_attack_up(void *d)    { in_attack  = 0.0f;  }

static void key_bind_f(void *d)
{
  if (cmd_argc() != 3) {
    log_printf(LOG_ERROR, "in_bind_f(): usage: %s [key] [command]", cmd_argv(0));
    return;
  }
  
  int key = cmd_argv(1)[0];
  const char *text = string_copy(cmd_argv(2)); 
  
  in_key_bind(key, text);
}

void in_init()
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
  cmd_add_command("+attack", in_attack_down, NULL);
  cmd_add_command("-attack", in_attack_up, NULL);
  
  cmd_add_command("bind", key_bind_f, NULL);
  
  in_forward  = 0.0f;
  in_left     = 0.0f;
  in_back     = 0.0f;
  in_right    = 0.0f;
  in_jump     = 0.0f;
  
  in_yaw      = 0.0f;
  in_pitch    = 0.0f;
}

void in_key_bind(int key, const char *text)
{
  if (in_num_keybinds + 1 > MAX_KEYBINDS) {
    log_printf(LOG_ERROR, "sys_bind(): ran out of bindings");
    return;
  }
  
  in_keybinds[in_num_keybinds].key = key;
  in_keybinds[in_num_keybinds].text = text;
  in_num_keybinds++;
}

void in_key_event(int key, int action)
{
  for (int i = 0; i < in_num_keybinds; i++) {
    if (in_keybinds[i].key == key) {
      if (in_keybinds[i].text[0] == '+') {
        if (action)
          cmd_puts("+");
        else
          cmd_puts("-");
        
        cmd_puts(&in_keybinds[i].text[1]);
      } else if (action) {
        cmd_puts(in_keybinds[i].text);
      }
      
      cmd_puts("\n");
    }
  }
}

void in_mouse_event(int button, int action)
{
  in_attack = action;
}

void in_mouse_move(int dx, int dy)
{
  in_yaw += dx * IN_SENSITIVITY;
  
  float new_pitch = in_pitch + dy * IN_SENSITIVITY;
  
  if (fabs(new_pitch) < MAX_PITCH)
    in_pitch = new_pitch;
}

void in_base_move(usercmd_t *usercmd)
{
  usercmd->forward  = in_forward - in_back;
  usercmd->right    = in_right - in_left;
  usercmd->jump     = in_jump;
  usercmd->attack   = in_attack;
  usercmd->yaw      = in_yaw;
  usercmd->pitch    = in_pitch;
}
