#include "../server/server.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
#endif

void  sys_init();
void  sys_update();
void  sys_quit();
int   sys_time();

void sys_init()
{
  sv_init();
}

static int prev_time;

void sys_update()
{
  int now_time = sys_time();
  int delta_time = now_time - prev_time;
  prev_time = now_time;
  
  sv_update(delta_time);
}

void sys_quit()
{
  exit(0);
}

int sys_time()
{
  struct timespec spec;
  clock_gettime(CLOCK_MONOTONIC, &spec);
  return spec.tv_sec * 1000 + spec.tv_nsec / 1.0e6;
}

int main(int argc, char *argv[])
{
  srand(time(0));
  
  sys_init();
  
  prev_time = sys_time();
  
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(sys_update, 0, true);
#else
  while (1)
    sys_update();
#endif
  
  return 0;
}
