#include "sv_local.h"

#include "../game/map_file.h"
#include "../common/log.h"
#include <stdbool.h>
#include <stdio.h>

server_t  sv;

int       host_frametime = 15;

void sv_init()
{
  sv.num_clients = 0;
  
  net_listen();
  sg_init(&sv.sg);
  
  sv_load_map("nk_flatgrass");
}

static int lag_time = 0;

void sv_update(int delta_time)
{
  lag_time += delta_time;
  
  while (lag_time > 0) {
    lag_time -= host_frametime;
    sv_fixed_update();
  }
}

void sv_fixed_update()
{
  sv_accept();
  sv_parse();
  sv_game_update();
  sv_send_snapshot();
}

void sv_game_update()
{
  for (int i = 0; i < sv.num_clients; i++) {
    sv_client_t *client = &sv.clients[i];
    
    if (client->cmd_tail < client->cmd_head)
      sg_set_cmd(&sv.sg, client->entity, sv_client_get_usercmd(client));
  }
  
  sg_update(&sv.sg);
}

void sv_load_map(const char *name)
{
  char map_path[256];
  sprintf(map_path, "assets/map/%s.map", name);
  
  map_t map;
  
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "cl_load_map(): failed to load %s", map_path);
  
  sg_new_map(&sv.sg, &map);
}
