#include "client.h"

#include "win.h"
#include "../common/map-file.h"
#include "../common/sys.h"
#include "../cgame/cgame.h"
#include "../renderer/gl.h"
#include "../renderer/renderer.h"

#ifdef EMSCRIPTEN
  #include <emscripten/emscripten.h>
#endif

static client_t   client;
static cgame_t    cgame;
static renderer_t renderer;

static void main_event_loop()
{
  sys_event_t *event;
  while ((event = sys_get_event())) {
    switch (event->type) {
    case SYS_KEY_PRESS:
      client_key_press(&client, event->data.key_press.key, event->data.key_press.action);
      break;
    case SYS_MOUSE_MOVE:
      client_mouse_move(&client, event->data.mouse_move.dx, event->data.mouse_move.dy);
      break;
    }
  }
}

static void main_init()
{
  if (!win_init(1280, 720, "nuko"))
    sys_log(SYS_FATAL, "main(): failed to initialise window");

#ifndef EMSCRIPTEN
  if (!gl_init())
    sys_log(SYS_FATAL, "main(): failed to initialise OpenGL");
#endif
  
  if (!renderer_init(&renderer))
    sys_log(SYS_FATAL, "main(): failed to initialize renderer");
  
  client_init(&client);
  cgame_init(&cgame);
  
  map_t map;
  
  if (!map_load(&map, "../../assets/map/untitled.map"))
    sys_log(SYS_FATAL, "main(): failed to load untitled.map");
  
  if (!renderer_new_map(&renderer, &map))
    sys_log(SYS_FATAL, "main(): renderer failed to load new map");
  
  cgame_new_map(&cgame, &map);
}

static void main_update()
{
  win_poll();
  
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
