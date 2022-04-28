#include "client.h"
#include "win.h"
#include "sys.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../game/map_file.h"
#include "../renderer/gl.h"

void cl_init(client_t *cl)
{
#ifndef __EMSCRIPTEN__
  if (!gl_init())
    log_printf(LOG_FATAL, "cl_init(): failed to initialise OpenGL");
#endif
  
  if (!renderer_init(&cl->renderer))
    log_printf(LOG_FATAL, "cl_init(): failed to initialize renderer");
  
  *cl = (client_t) {0};
  
  cl_net_init(cl);
  cl_input_init(cl);
  cg_init(&cl->cg);
  renderer_init(&cl->renderer);
  
  cl_load_map(cl, "nk_construct");
}

void cl_update(client_t *cl)
{
  cl_net_recv(cl);
  cl_base_move(cl);
  cl_send_cmd(cl);
  cl_predict(cl);
  
  renderer_render_player_view(&cl->renderer, &cl->cg);
}

void cl_predict(client_t *cl)
{
  cg_reconcile(&cl->cg, &cl->snapshot);
  
  for (int i = cl->incoming_ack + 1; i < cl->outgoing_seq; i++) {
    cg_set_cmd(&cl->cg, &cl->cmd_queue[i % MAX_USERCMDS]);
    cg_update(&cl->cg);
  }
}

void cl_load_map(client_t *cl, const char *name)
{
  char map_path[256];
  sprintf(map_path, "assets/map/%s.map", name);
  
  map_t map;
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "cl_load_map(): failed to load %s", map_path);
  
  if (!renderer_new_map(&cl->renderer, &map))
    log_printf(LOG_FATAL, "cl_load_map(): renderer failed to load new map");
  
  cg_new_map(&cl->cg, &map);
}
