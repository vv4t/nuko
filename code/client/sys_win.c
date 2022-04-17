#include "win.h"

#include "sys.h"
#include "../common/log.h"
#include <SDL2/SDL.h>

static SDL_Window     *win_sdl_window;
static SDL_GLContext  win_gl_context;

bool sys_win_init(int width, int height, const char *title)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  
  SDL_SetRelativeMouseMode(true);
  
  win_sdl_window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width,
    height,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  
  if (!win_sdl_window) {
    log_printf(LOG_ERROR, "win_init(): failed to create SDL window");
    return false;
  }
  
  win_gl_context = SDL_GL_CreateContext(win_sdl_window);
  
  if (!win_gl_context) {
    log_printf(LOG_ERROR, "win_init(): failed to create GL context");
    return false;
  }
  
  return true;
}

void sys_win_poll()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    sys_event_t sys_event = {0};
    switch (event.type) {
    case SDL_QUIT:
      sys_event.type = SYS_QUIT;
      break;
    case SDL_KEYUP:
      sys_event.type = SYS_KEY_PRESS;
      sys_event.data.key_press.key = event.key.keysym.sym;
      sys_event.data.key_press.action = 0;
      break;
    case SDL_KEYDOWN:
      sys_event.type = SYS_KEY_PRESS;
      sys_event.data.key_press.key = event.key.keysym.sym;
      sys_event.data.key_press.action = 1;
      break;
    case SDL_MOUSEMOTION:
      sys_event.type = SYS_MOUSE_MOVE;
      sys_event.data.mouse_move.dx = event.motion.xrel;
      sys_event.data.mouse_move.dy = event.motion.yrel;
      break;
    }
    
    sys_queue_event(&sys_event);
  }
}

void sys_win_quit()
{
  SDL_GL_DeleteContext(win_gl_context);
  SDL_DestroyWindow(win_sdl_window);
  SDL_Quit();
}

void sys_win_swap()
{
  SDL_GL_SwapWindow(win_sdl_window);
}

