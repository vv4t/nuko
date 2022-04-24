#include "sv_local.h"

#include "../game/map_file.h"
#include "../common/log.h"
#include <stdbool.h>
#include <stdio.h>

void sv_init(server_t *sv)
{
  *sv = (server_t) {0};
  
  net_listen(8000);
  sg_init(&sv->sg);
  
  const char *map_path = "assets/map/nk_construct.map";
  
  map_t map;
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "client_load_map(): failed to load %s", map_path);
  
  sg_new_map(&sv->sg, &map);
}

void sv_update(server_t *sv)
{
  sv_cull(sv);
  sv_accept(sv);
  sv_poll(sv);
  
  for (int i = 0; i < sv->num_clients; i++) {
    sv_client_t *client = &sv->clients[i];
    frame_t *frame = &client->frame_queue[(client->frame_tail++) % MAX_FRAME_QUEUE];
    sv->sg.bg.client[client->entity].usercmd = frame->data.usercmd;
    client->incoming_seq = frame->outgoing_seq;
  }
  
  sg_update(&sv->sg);
  
  sv_send_all_snapshot(sv);
}
