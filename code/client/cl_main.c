#include "cl_local.h"

client_t    cl;
cgame_t     cg;

static int  cl_cmdrate  = 50;

#ifdef __EMSCRIPTEN__
void cl_get_host_address(char *host_address, int len);
#else
void cl_get_host_address(char *host_address, int len)
{
  const char *str_host = "127.0.0.1";
  memcpy(host_address, str_host, strlen(str_host) + 1);
}
#endif

void say_f(void *d)
{
  if (cmd_argc() != 2) {
    log_printf(LOG_ERROR, "say_f(): usage: %s [msg]", cmd_argv(0));
    return;
  }
  
  cl_send_chat(cmd_argv(1));
}

void name_f(void *d)
{
  if (cmd_argc() != 2) {
    log_printf(LOG_ERROR, "name_f(): usage: %s [name]", cmd_argv(0));
    return;
  }
  
  cl_send_name(cmd_argv(1));
}

void cl_init()
{
  cmd_add_command("say", say_f, NULL);
  cmd_add_command("name", name_f, NULL);
  
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
}

void cl_console()
{
  const char *in_cmd = sys_read_in();
  
  if (in_cmd) {
    if (in_cmd[0] == '!') {
      cmd_puts(&in_cmd[1]);
      cmd_puts("\n");
      cmd_execute();
    } else {
      cl_send_chat(in_cmd);
    }
  }
}

static int  next_cmd = 0;

void cl_update(int delta_time)
{
  in_base_move(&cl.usercmd);
  
  float interp = ((float) next_cmd / (float) cl_cmdrate);
  
  cl_console();
  cl_view_look();
  cl_interpolate(interp);  
  cl_parse();
  
  next_cmd += delta_time;
  
  if (next_cmd > cl_cmdrate) {
    next_cmd = next_cmd % cl_cmdrate;
    
    cl_send_cmd();
    cl_predict();
    cl_snapshot();
  }
  
  r_render_client_view();
}
