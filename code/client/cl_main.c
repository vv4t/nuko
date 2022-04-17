#include "client.h"
#include "win.h"
#include "sys.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../game/map-file.h"
#include "../renderer/gl.h"

void client_init(client_t *client)
{
#ifndef EMSCRIPTEN
  if (!gl_init())
    log_printf(LOG_FATAL, "client_init(): failed to initialise OpenGL");
#endif
  
  if (!renderer_init(&client->renderer))
    log_printf(LOG_FATAL, "client_init(): failed to initialize renderer");
  
  *client = (client_t) {0};
  
  client_net_init(client);
  client_input_init(client);
  cgame_init(&client->cgame);
  renderer_init(&client->renderer);
  
  client_load_map(client, "nk_flatgrass");
}

void client_update(client_t *client)
{
  client_base_move(client);
  
  client_send_cmd(client);
  cgame_send_cmd(&client->cgame, &client->usercmd);
  
  cgame_update(&client->cgame);
  
  renderer_render_player_view(&client->renderer, &client->cgame);
}

void client_load_map(client_t *client, const char *name)
{
  char map_path[256];
  sprintf(map_path, "assets/map/%s.map", name);
  
  map_t map;
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "client_load_map(): failed to load %s", map_path);
  
  if (!renderer_new_map(&client->renderer, &map))
    log_printf(LOG_FATAL, "client_load_map(): renderer failed to load new map");
  
  cgame_new_map(&client->cgame, &map);
}
