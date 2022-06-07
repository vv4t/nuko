#include "sv_local.h"

server_t  sv;

int       host_frametime = 50; // Time between frames

void sv_init()
{
  // Reset state
  
  sv.snapshot_head = 0;
  sv.num_clients = 0;
  sv.round_count = 0;
  sv.round_time = 0;
  sv.round_start = false;
  
  edict_init(&sv.edict);
  bg_init(&sv.bg, &sv.edict);
  
  // Begin listening for clients
  net_listen();
  
  // Default map
  sv_load_map("nk_yuu");
}

static int next_frame = 0;

void sv_update(int delta_time)
{
  next_frame += delta_time;
  
  while (next_frame > 0) {
    next_frame -= host_frametime;
    
    sv_accept();
    sv_parse();
    sv_game_update();
    sv_send_snapshot();
  }
}
