#include "sys.h"

#include "server.h"
#include <stdlib.h>
#include <stdbool.h>

#define FRAMES_PER_SEC 60

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
#else
  #include <time.h>
  #define SECS_PER_FRAME (1.0f / ((double) FRAMES_PER_SEC))
  #define CLOCKS_PER_FRAME ((clock_t) ((double) CLOCKS_PER_SEC * (double) SECS_PER_FRAME))
#endif

static server_t sv;

void sys_init()
{
  sv_init(&sv);
}

void sys_update()
{
  sv_update(&sv);
}

void sys_quit()
{
  exit(0);
}

int main(int argc, char *argv[])
{
  sys_init();
  
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(sys_update, FRAMES_PER_SEC, true);
#else
  clock_t prev_time, lag_time;
  
  lag_time = 0;
  prev_time = clock();
  
  int tick = 0;
  
  while (1) {
    clock_t now_time = clock();
    clock_t delta_time = now_time - prev_time;
    prev_time = now_time;
    
    lag_time += delta_time;
    
    if (lag_time >= 0) {
      lag_time -= CLOCKS_PER_FRAME;
      
      tick++;
      sys_update();
    }
  }
#endif
  
  return 0;
}
