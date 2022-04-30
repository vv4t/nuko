#include "sys.h"

#include "client.h"
#include "input.h"
#include "gl.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../common/string.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH           640
#define HEIGHT          480

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
#endif

static void console_input_f(void *d)
{
  char text_buf[256];
  
  printf(":");
  const char *text = fgets(text_buf, 256, stdin);
  
  cmd_puts(text);
}

void sys_config()
{
  cmd_add_command("open_console", console_input_f, NULL);
  
  in_key_bind('w', "+forward");
  in_key_bind('a', "+left");
  in_key_bind('s', "+back");
  in_key_bind('d', "+right");
  in_key_bind(' ', "+jump");
  in_key_bind('`', "open_console");
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

int main(int argc, char* argv[])
{
  in_init();
  sys_config();
  
  if (!sys_win_init(WIDTH, HEIGHT, "nuko"))
    log_printf(LOG_FATAL, "main(): failed to initialise window");
  
  if (!gl_init())
    log_printf(LOG_FATAL, "main(): failed to initialise OpenGL");
  
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
