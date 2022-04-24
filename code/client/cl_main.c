#include "client.h"
#include "win.h"
#include "sys.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../game/map_file.h"
#include "../renderer/gl.h"

void cl_init(client_t *client)
{
#ifndef __EMSCRIPTEN__
  if (!gl_init())
    log_printf(LOG_FATAL, "cl_init(): failed to initialise OpenGL");
#endif
  
  if (!renderer_init(&client->renderer))
    log_printf(LOG_FATAL, "cl_init(): failed to initialize renderer");
  
  *client = (client_t) {0};
  
  cl_net_init(client);
  cl_input_init(client);
  cg_init(&client->cg);
  renderer_init(&client->renderer);
  
  cl_load_map(client, "nk_construct");
}

void cl_update(client_t *client)
{
  cl_poll(client);
  
  cl_base_move(client);
  cg_send_cmd(&client->cg, &client->usercmd);
  cl_send_cmd(client);
  
  cg_update(&client->cg);
  
  renderer_render_player_view(&client->renderer, &client->cg);
}

void cl_load_map(client_t *client, const char *name)
{
  char map_path[256];
  sprintf(map_path, "assets/map/%s.map", name);
  
  map_t map;
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "cl_load_map(): failed to load %s", map_path);
  
  if (!renderer_new_map(&client->renderer, &map))
    log_printf(LOG_FATAL, "cl_load_map(): renderer failed to load new map");
  
  cg_new_map(&client->cg, &map);
}
