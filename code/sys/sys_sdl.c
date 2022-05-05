#include "../client/client.h"
#include "../client/input.h"
#include "../client/gl.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../common/string.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdbool.h>

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
  int main_gets(char *buf, int len);
  void main_focus_input();
#endif

#define NUKO_WIDTH   1280
#define NUKO_HEIGHT  720
#define NUKO_TITLE   "nuko"

void  sys_config();
int   sys_time();
void  sys_quit();
void  sys_poll();
void  sys_update();
bool  sys_init_win();
void  sys_poll();
void  sys_quit();
void  sys_swap();
void  sys_focus();
void  sys_unfocus();

static SDL_Window     *win_sdl_window;
static SDL_GLContext  win_gl_context;

static bool           sys_focused = false;

static void console_input_f(void *d)
{
#ifdef __EMSCRIPTEN__
  sys_unfocus();
  main_focus_input();
#else
  char text_buf[256];
  
  printf(":");
  const char *text = fgets(text_buf, 256, stdin);
  
  cmd_puts(text);
#endif
}

void sys_config()
{
  cmd_add_command("open_console", console_input_f, NULL);
  
  in_key_bind('w', "+forward");
  in_key_bind('a', "+left");
  in_key_bind('s', "+back");
  in_key_bind('d', "+right");
  in_key_bind(' ', "+jump");
  in_key_bind('y', "open_console");
}

int sys_time()
{
  struct timespec spec;
  clock_gettime(CLOCK_MONOTONIC, &spec);
  return spec.tv_sec * 1000 + spec.tv_nsec / 1.0e6;
}

static int prev_time;

void sys_update()
{
  int now_time = sys_time();
  int delta_time = now_time - prev_time;
  prev_time = now_time;
  
  sys_poll();
  cl_update(delta_time);
  sys_swap();
}

bool sys_init_win()
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
  
  win_sdl_window = SDL_CreateWindow(
    NUKO_TITLE,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    NUKO_WIDTH,
    NUKO_HEIGHT,
    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  
  if (!win_sdl_window) {
    log_printf(LOG_ERROR, "sys_init_win(): failed to create SDL window");
    return false;
  }
  
  win_gl_context = SDL_GL_CreateContext(win_sdl_window);
  
  if (!win_gl_context) {
    log_printf(LOG_ERROR, "sys_init_win(): failed to create GL context");
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
      if (!sys_focused)
        sys_focus();
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

#ifdef __EMSCRIPTEN__
  char text[128];
  int len = main_gets(text, 256);
  if (len) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "say \"%s\"", text);
    cmd_puts(cmd);
  }
#endif
  
  cmd_execute();
}

void sys_focus()
{
  SDL_EventState(SDL_TEXTINPUT, SDL_ENABLE);
  SDL_EventState(SDL_KEYDOWN, SDL_ENABLE);
  SDL_EventState(SDL_KEYUP, SDL_ENABLE);
  
  SDL_SetRelativeMouseMode(true);
  
  sys_focused = true;
}

void sys_unfocus()
{
  SDL_EventState(SDL_TEXTINPUT, SDL_DISABLE);
  SDL_EventState(SDL_KEYDOWN, SDL_DISABLE);
  SDL_EventState(SDL_KEYUP, SDL_DISABLE);
  
  SDL_SetRelativeMouseMode(false);
  
  sys_focused = false;
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

int main(int argc, char* argv[])
{
  in_init();
  sys_config();
  
  if (!sys_init_win())
    log_printf(LOG_FATAL, "main(): failed to initialise window");
  
  if (!gl_init())
    log_printf(LOG_FATAL, "main(): failed to initialise OpenGL");
  
  sys_focus();
  
  cl_init();
  
  prev_time = sys_time();

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(sys_update, 0, true);
#else
  while (1)
    sys_update();
#endif
  
  sys_quit();
  
  return 0;
}
