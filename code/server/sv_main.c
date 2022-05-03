#include "sv_local.h"

server_t    sv;

static int  host_frametime = 50;

void sv_init()
{
  sv.snapshot_head = 0;
  
  edict_init(&sv.edict);
  bg_init(&sv.bg, &sv.edict);
  
  net_listen();
  
  sv_load_map("nk_flatgrass");
}

static int lag_time = 0;

void sv_update(int delta_time)
{
  lag_time += delta_time;
  
  while (lag_time > 0) {
    lag_time -= host_frametime;
    
    sv_accept();
    sv_parse();
    sv_game_update();
    sv_send_snapshot();
  }
}
