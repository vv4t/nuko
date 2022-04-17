#include <stdio.h>

#include "../common/net.h"
#include "../game/bgame.h"
#include <stdbool.h>

#ifdef EMSCRIPTEN
  #include <emscripten.h>
#endif

typedef struct {
  edict_t edict;
  bgame_t bgame;
} sgame_t;

static sgame_t  sgame;

void sgame_init(sgame_t *sgame)
{
  edict_init(&sgame->edict);
  bgame_init(&sgame->bgame, &sgame->edict);
}

void sv_init()
{
  net_listen(8000);
  sgame_init(&sgame);
}

void sv_update()
{
  netchan_t netchan;
  while (net_accept(&netchan))
    printf("new client %i\n", netchan.sock);
}

int main(int argc, char *argv[])
{
  sv_init();
  
#ifdef EMSCRIPTEN
  emscripten_set_main_loop(sv_update, 0, true);
#else
  while (1)
    sv_update();
#endif
}
