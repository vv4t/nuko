#include "cl_local.h"

#include "sys.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../game/map_file.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

client_t  cl;

int       host_frametime = 15;

#ifdef __EMSCRIPTEN__
void cl_get_host_address(char *host_address, int len);
#else
void cl_get_host_address(char *host_address, int len)
{
  const char *str_host = "127.0.0.1";
  memcpy(host_address, str_host, strlen(str_host) + 1);
}
#endif

void cl_init()
{
  cl.connected = false;
  cl.cmd_tail = 0;
  cl.cmd_head = 0;
  
  cg_init(&cl.cg);
  
  if (!r_init())
    log_printf(LOG_FATAL, "cl_init(): failed to initialize renderer");
  
  char host_address[256];
  cl_get_host_address(host_address, 256);
  
  cl_connect(host_address);
  cl_load_map("nk_flatgrass");
}

static int lag_time = 0;

void cl_update(int delta_time)
{
  lag_time += delta_time;
  
  while (lag_time > 0) {
    lag_time -= host_frametime;
    cl_fixed_update();
  }
}

void cl_fixed_update()
{
  cl_parse();
  if (!cl.connected)
    return;
  
  cl_send_cmd();
  cl_predict();
  
  r_render_player_view(&cl.cg);
}

void cl_predict()
{
  cg_reconcile(&cl.cg, &cl.snapshot);
  
  for (int i = cl.cmd_tail + 1; i < cl.cmd_head; i++) {
    cg_set_cmd(&cl.cg, &cl.cmd_queue[i % MAX_CMD_QUEUE]);
    cg_update(&cl.cg);
  }
}

void cl_load_map(const char *name)
{
  char map_path[256];
  sprintf(map_path, "assets/map/%s.map", name);
  
  map_t map;
  
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "cl_load_map(): failed to load %s", map_path);
  
  if (!r_new_map(&map))
    log_printf(LOG_FATAL, "cl_load_map(): renderer failed to load new map");
  
  cg_new_map(&cl.cg, &map);
}
