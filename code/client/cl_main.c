#include "cl_local.h"

client_t    cl;
cgame_t     cg;

static int  host_frametime  = 50;
static int  host_framecount = 0;

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
  
  edict_init(&cg.edict);
  bg_init(&cg.bg, &cg.edict);
  
  if (!r_init())
    log_printf(LOG_FATAL, "cl_init(): failed to initialize renderer");
  
  char host_address[256];
  cl_get_host_address(host_address, 256);
  
  cl_connect(host_address);
  cl_load_map("nk_flatgrass");
}

static int next_frame = 0;

void cl_update(int delta_time)
{
  in_base_move(&cl.usercmd);
  
  float interp = ((float) next_frame / (float) host_frametime) + 1.0f;
  
  cl_view_look();
  cl_interpolate(interp);  
  
  next_frame += delta_time;
  if (next_frame > 0) {
    next_frame -= host_frametime;
    
    cl_parse();
    cl_send_cmd();
    cl_predict();
    
    cl_snapshot();
    
    host_framecount++;
    
    if (host_framecount % (1000 / host_frametime) == 0) {
      printf("cmd_queue: %i, cmd_head: %i, cmd_tail: %i\n", cl.cmd_head - cl.cmd_tail, cl.cmd_head, cl.cmd_tail);
    }
  }
  
  r_render_client_view();
}
