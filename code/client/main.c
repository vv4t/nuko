#include "client.h"
#include "win.h"
#include "sys.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../game/map-file.h"
#include "../cgame/cgame.h"
#include "../renderer/gl.h"
#include "../renderer/renderer.h"

#ifdef EMSCRIPTEN
  #include <emscripten/emscripten.h>
#endif

static client_t   client;
static cgame_t    cgame;
static renderer_t renderer;

static void load_map(const char *name)
{
  char map_path[256];
  sprintf(map_path, "../../assets/map/%s.map", name);
  
  map_t map;
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "load_map(): failed to load %s", map_path);
  
  if (!renderer_new_map(&renderer, &map))
    log_printf(LOG_FATAL, "load_map(): renderer failed to load new map");
  
  cgame_new_map(&cgame, &map);
}

static void load_map_f(void *d)
{
  if (cmd_argc() != 2) {
    log_printf(LOG_ERROR, "load_map_f(): usage: %s [map_name]", cmd_argv(0));
    return;
  }
  
  load_map(cmd_argv(1));
}

static void main_init_binds()
{
  cmd_add_command("map", load_map_f, NULL);
  
  sys_bind("open_console", '`');
  
  sys_bind("+forward", 'w');
  sys_bind("+left", 'a');
  sys_bind("+back", 's');
  sys_bind("+right", 'd');
  sys_bind("+jump", ' ');
}

static void main_event_loop()
{
  sys_event_t *event;
  while ((event = sys_get_event())) {
    switch (event->type) {
    case SYS_KEY_PRESS:
      break;
    case SYS_MOUSE_MOVE:
      client_mouse_move(&client, event->data.mouse_move.dx, event->data.mouse_move.dy);
      break;
    }
  }
}

static void main_init()
{
  cmd_init();
  sys_init();
  
  main_init_binds();
  
  if (!win_init(1280, 720, "nuko"))
    log_printf(LOG_FATAL, "main(): failed to initialise window");

#ifndef EMSCRIPTEN
  if (!gl_init())
    log_printf(LOG_FATAL, "main(): failed to initialise OpenGL");
#endif
  
  if (!renderer_init(&renderer))
    log_printf(LOG_FATAL, "main(): failed to initialize renderer");
  
  win_cursor_lock(true);
  
  client_init(&client);
  cgame_init(&cgame);
  
  load_map("nk_flatgrass");
}

static void main_update()
{
  win_poll();
  cmd_execute();
  
  main_event_loop();
  
  client_base_move(&client);
  
  cgame_send_cmd(&cgame, &client.usercmd);
  cgame_update(&cgame);
  
  renderer_render_player_view(&renderer, &cgame);
  
  win_swap();
}

static void main_quit()
{
  win_quit();
}

int main(int argc, char* argv[])
{
  main_init();

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(main_update, 0, true);
#else
  while (!win_should_quit())
    main_update();
#endif
  
  main_quit();
  
  return 0;
}
