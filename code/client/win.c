#include "win.h"

#include "../common/sys.h"
#include <SDL2/SDL.h>

static bool win_is_quit;
static SDL_Window *win_sdl_window;
static SDL_GLContext win_gl_context;

bool win_init(int width, int height, const char *title)
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
  
  SDL_SetRelativeMouseMode(SDL_TRUE);
  
  win_is_quit = false;
  
  win_sdl_window = SDL_CreateWindow(
    title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width,
    height,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  
  if (!win_sdl_window) {
    sys_log(SYS_ERROR, "win_init(): failed to create SDL window");
    return false;
  }
  
  win_gl_context = SDL_GL_CreateContext(win_sdl_window);
  
  if (!win_gl_context) {
    sys_log(SYS_ERROR, "win_init(): failed to create GL context");
    return false;
  }
  
  return true;
}

void win_poll()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      win_is_quit = true;
      break;
    case SDL_KEYUP:
      sys_key_press(event.key.keysym.sym, 0);
      break;
    case SDL_KEYDOWN:
      sys_key_press(event.key.keysym.sym, 1);
      break;
    case SDL_MOUSEMOTION:
      sys_mouse_move(event.motion.xrel, event.motion.yrel);
      break;
    }
  }
}

void win_quit()
{
  SDL_GL_DeleteContext(win_gl_context);
  SDL_DestroyWindow(win_sdl_window);
  SDL_Quit();
}

void win_swap()
{
  SDL_GL_SwapWindow(win_sdl_window);
}

bool win_should_quit()
{
  return win_is_quit;
}
