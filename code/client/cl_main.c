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

static cgame_t cg;
static renderer_t r;
static usercmd_t usercmd;

void cl_event_loop()
{
  sys_event_t *event;
  while ((event = sys_get_event())) {
    switch (event->type) {
    case SYS_KEY_PRESS:
      cl_key_press(event->data.key_press.key, event->data.key_press.action);
      break;
    case SYS_MOUSE_MOVE:
      cl_mouse_move(event->data.mouse_move.dx, event->data.mouse_move.dy);
      break;
    }
  }
}

void main_init()
{
  if (!win_init(1280, 720, "nuko"))
    sys_log(SYS_FATAL, "main(): failed to initialise window");

#ifndef EMSCRIPTEN
  if (!gl_init())
    sys_log(SYS_FATAL, "main(): failed to initialise OpenGL");
#endif
  
  if (!r_init(&r))
    sys_log(SYS_FATAL, "main(): failed to initialize renderer");
  
  cg_init(&cg);
  
  map_t map;
  if (!map_load(&map, "../../assets/map/untitled.map"))
    sys_log(SYS_FATAL, "main(): failed to load untitled.map");
  
  r_new_map(&r, &map);
  cg_new_map(&cg, &map);
}

void main_update()
{
  win_poll();
  
  cl_event_loop();
  cl_base_move(&usercmd);
  
  cg_send_cmd(&cg, &usercmd);
  cg_update(&cg);
  r_render_player_view(&r, &cg);
  
  win_swap();
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
  
  win_quit();
  
  return 0;
}
