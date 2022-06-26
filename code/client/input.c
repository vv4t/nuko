#include "input.h"

#define MAX_PITCH       1.57 // Angle in radians
#define MAX_KEYBINDS    32

typedef struct {
  int         key;
  const char  *text;
} keybind_t;

// Console bindings
static keybind_t    in_keybinds[MAX_KEYBINDS];
static int          in_num_keybinds = 0;

// Input states
static float        in_forward;
static float        in_left;
static float        in_back;
static float        in_right;
static float        in_jump;
static float        in_attack1;
static float        in_attack2;

// Camera orientation based on mouse movement
static float        in_pitch;
static float        in_yaw;

static int          in_weapon_slot;

// Rotation sensitivity
static float        in_sensitivity = 0.005;

// Command callback functions
// When called, the corresponding input state is modified
static void in_forward_down(void *d)  { in_forward      = 1.0f; }
static void in_forward_up(void *d)    { in_forward      = 0.0f; }
static void in_left_down(void *d)     { in_left         = 1.0f; }
static void in_left_up(void *d)       { in_left         = 0.0f; }
static void in_back_down(void *d)     { in_back         = 1.0f; }
static void in_back_up(void *d)       { in_back         = 0.0f; }
static void in_right_down(void *d)    { in_right        = 1.0f; }
static void in_right_up(void *d)      { in_right        = 0.0f; }
static void in_jump_down(void *d)     { in_jump         = 1.0f; }
static void in_jump_up(void *d)       { in_jump         = 0.0f; }
static void in_attack1_down(void *d)  { in_attack1      = 1.0f; }
static void in_attack1_up(void *d)    { in_attack1      = 0.0f; }
static void in_attack2_down(void *d)  { in_attack2      = 1.0f; }
static void in_attack2_up(void *d)    { in_attack2      = 0.0f; }
static void in_weapon_slot_1(void *d) { in_weapon_slot  = 0;    }
static void in_weapon_slot_2(void *d) { in_weapon_slot  = 1;    }

// Bind a key to a certain command
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

// Set the sensitivty
static void sensitivity_f(void *d)
{
  switch (cmd_argc()) {
  case 1: // If there are no arguments, print the current sensitivity
    printf("%f\n", in_sensitivity);
    break;
  case 2:
    in_sensitivity = atof(cmd_argv(1));
    printf("sensitivity set to '%f'\n", atof(cmd_argv(1)));
    break;
  default:
    log_printf(LOG_ERROR, "sensitivity_f(): usage %s [sensitivity]", cmd_argv(0));
    break;
  }
}

void in_init()
{
  // Bind commands to respective callback functions
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
  cmd_add_command("+attack1", in_attack1_down, NULL);
  cmd_add_command("-attack1", in_attack1_up, NULL);
  cmd_add_command("+attack2", in_attack2_down, NULL);
  cmd_add_command("-attack2", in_attack2_up, NULL);
  
  cmd_add_command("weapon_slot_1", in_weapon_slot_1, NULL);
  cmd_add_command("weapon_slot_2", in_weapon_slot_2, NULL);
  
  // Add commands to configure input
  cmd_add_command("sensitivity", sensitivity_f, NULL);
  cmd_add_command("bind", key_bind_f, NULL);
  
  // Reset the input state
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
  // Boundary checks
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
      if (in_keybinds[i].text[0] == '+') { // Binds beginning with '+' correspond to key events
        if (action)
          cmd_puts("+");
        else // The key was released
          cmd_puts("-");
        
        cmd_puts(&in_keybinds[i].text[1]);
        
        // NOTE: this is a somewhat dodgy implementation. It uses the fact that
        // it only registers as a command between '\n' or '\0' characters. This
        // This means you can do things like:
        //
        // cmd_puts("hi ")
        // cmd_puts("there")
        // cmd_puts("\n")
        //
        // And it would still register as 'hi there\n'
        // This section abuses this to add '+' or '-' based on the key press
        // before adding the actual command. This allows it to concatenate the
        // values without creating a new string in this function.
        // I'd rather this be done in a single function though
        // Perhaps in a variadic function like: cmd_puts("%c%s\n", action ? '+', '-', cmd_str);
      } else if (action) {
        cmd_puts(in_keybinds[i].text);
      }
      
      cmd_puts("\n");
    }
  }
}

void in_mouse_event(int button, int action)
{
  switch (button) {
  case 1:
    in_attack1 = action;
    break;
  case 3:
    in_attack2 = action;
    break;
  }
}

void in_mouse_move(int dx, int dy)
{
  in_yaw += dx * in_sensitivity;
  
  float new_pitch = in_pitch + dy * in_sensitivity;
  
  if (fabs(new_pitch) < MAX_PITCH) // Limit how much you can look down
    in_pitch = new_pitch;
}

void in_base_move(usercmd_t *usercmd)
{
  usercmd->forward  = in_forward - in_back;
  usercmd->right    = in_right - in_left;
  usercmd->jump     = in_jump;
  usercmd->attack1  = in_attack1;
  usercmd->attack2  = in_attack2;
  usercmd->yaw      = in_yaw;
  usercmd->pitch    = in_pitch;
  usercmd->weapon_slot = in_weapon_slot;
}
