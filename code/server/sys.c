#include "sys.h"

#include "server.h"
#include <stdlib.h>
#include <stdbool.h>

#ifdef EMSCRIPTEN
  #include <emscripten.h>
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
  
#ifdef EMSCRIPTEN
  emscripten_set_main_loop(sys_update, 60, true);
#else
  while (1)
    sys_update();
#endif
  
  return 0;
}
