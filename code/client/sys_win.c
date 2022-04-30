#include "sys.h"

#include "input.h"
#include "../common/log.h"
#include "../common/cmd.h"
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
    NUKO_TITLE,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    NUKO_WIDTH,
    NUKO_HEIGHT,
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

void sys_poll()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      sys_quit();
      break;
    case SDL_KEYUP:
      in_key_event(event.key.keysym.sym, 0);
      break;
    case SDL_KEYDOWN:
      in_key_event(event.key.keysym.sym, 1);
      break;
    case SDL_MOUSEBUTTONDOWN:
      in_mouse_event(0, 1);
      break;
    case SDL_MOUSEBUTTONUP:
      in_mouse_event(0, 0);
      break;
    case SDL_MOUSEMOTION:
      in_mouse_move(event.motion.xrel, event.motion.yrel);
      break;
    }
  }
  
  cmd_execute();
}

void sys_quit()
{
  SDL_GL_DeleteContext(win_gl_context);
  SDL_DestroyWindow(win_sdl_window);
  SDL_Quit();
  exit(0);
}

void sys_swap()
{
  SDL_GL_SwapWindow(win_sdl_window);
}
