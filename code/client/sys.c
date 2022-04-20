#include "sys.h"

#include "client.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../common/string.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef EMSCRIPTEN
  #include <emscripten/emscripten.h>
#endif

typedef struct {

} server_t;

static client_t client;

static void load_map_f(void *d)
{
  if (cmd_argc() != 2) {
    log_printf(LOG_ERROR, "load_map_f(): usage: %s [map_name]", cmd_argv(0));
    return;
  }
  
  cl_load_map(&client, cmd_argv(1));
}

void key_bind_f(void *d)
{
  if (cmd_argc() != 3) {
    log_printf(LOG_ERROR, "bind_f(): usage %s [key] [command]", cmd_argv(0));
    return;
  }
  
  int key = cmd_argv(1)[0];
  const char *text = string_copy(cmd_argv(2)); 
  
  sys_bind(text, key);
}

static void sys_config()
{
  cmd_add_command("bind", key_bind_f, NULL);
  cmd_add_command("map", load_map_f, NULL);
  cmd_add_command("open_console", sys_console_input, NULL);
  
  sys_bind("+forward", 'w');
  sys_bind("+left", 'a');
  sys_bind("+back", 's');
  sys_bind("+right", 'd');
  sys_bind("+jump", ' ');
  sys_bind("open_console", '`');
}

void sys_event_loop()
{
  sys_event_t *event;
  while ((event = sys_get_event())) {
    switch (event->type) {
    case SYS_KEY_PRESS:
      sys_console_key_press(event->data.key_press.key, event->data.key_press.action);
      break;
    case SYS_MOUSE_MOVE:
      cl_mouse_move(&client, event->data.mouse_move.dx, event->data.mouse_move.dy);
      break;
    case SYS_QUIT:
      sys_quit();
      break;
    }
  }
}

void sys_init()
{
  cmd_init();
  sys_config();
  
  if (!sys_win_init(1280, 720, "nuko"))
    log_printf(LOG_FATAL, "sys_init(): failed to initialise window");
  
  cl_init(&client);
}

void sys_update()
{
  sys_win_poll();
  cmd_execute();
  sys_event_loop();
  cl_update(&client);
  sys_win_swap();
}

void sys_quit()
{
  sys_win_quit();
  exit(0);
}

int main(int argc, char* argv[])
{
  sys_init();

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(sys_update, 60, true);
#else
  while (1)
    sys_update();
#endif
  
  sys_quit();
  
  return 0;
}
