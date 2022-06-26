#include "cl_local.h"

client_t    cl;
cgame_t     cg;

// Rate at which to send usercmds
// NOTE: althought it is named rate it is similar to host_frametime, in that
// it is the milliseconds between sending each usercmd.
static int  cl_cmdrate  = 50;

// Write the host's address to a buffer
// NOTE: On browser, it simply connects to /socket. A dedicated PC version is
// not supported yet so it simply connects to localhost for testing purposes.
#ifdef __EMSCRIPTEN__
void cl_get_host_address(char *host_address, int len);
#else
void cl_get_host_address(char *host_address, int len)
{
  const char *str_host = "127.0.0.1";
  memcpy(host_address, str_host, strlen(str_host) + 1);
}
#endif

// Console function for chat
void say_f(void *d)
{
  if (cmd_argc() != 2) {
    log_printf(LOG_ERROR, "say_f(): usage: %s [msg]", cmd_argv(0));
    return;
  }
  
  cl_send_chat(cmd_argv(1));
}

// Console function for changing name
void name_f(void *d)
{
  if (cmd_argc() != 2) {
    log_printf(LOG_ERROR, "name_f(): usage: %s [name]", cmd_argv(0));
    return;
  }
  
  cl_send_name(cmd_argv(1));
}

// Console function for requesting scoreboard
void score_f(void *d)
{
  if (cmd_argc() != 1) {
    log_printf(LOG_ERROR, "score_f(): usage: %s");
    return;
  }
  
  cl_send_score();
}

void cl_init()
{
  cl.local = false;
  
  // Add client console commands
  cmd_add_command("say", say_f, NULL);
  cmd_add_command("name", name_f, NULL);
  cmd_add_command("score", score_f, NULL);
  
  // Reset usercmd cache
  cl.connected = false;
  cl.cmd_tail = 0;
  cl.cmd_head = 0;
  
  // Initialize game state
  edict_init(&cg.edict);
  bg_init(&cg.bg, &cg.edict);
  
  // Initialize renderer
  if (!r_init())
    log_printf(LOG_FATAL, "cl_init(): failed to initialize renderer");
  
  // Connect to server
  char host_address[256];
  cl_get_host_address(host_address, 256);
  
  cl_connect(host_address);
}

void cl_init_local()
{
  cl.local = true;
  
  // Add client console commands
  cmd_add_command("say", say_f, NULL);
  cmd_add_command("name", name_f, NULL);
  cmd_add_command("score", score_f, NULL);
  
  // Reset usercmd cache
  cl.connected = false;
  cl.cmd_tail = 0;
  cl.cmd_head = 0;
  
  // Initialize game state
  edict_init(&cg.edict);
  bg_init(&cg.bg, &cg.edict);
  
  // Initialize renderer
  if (!r_init())
    log_printf(LOG_FATAL, "cl_init(): failed to initialize renderer");
  
  cg.ent_client = edict_add_entity(&cg.edict, BG_ES_CLIENT & (~BGC_MODEL)); // Create a new entity
  cg.bg.transform[cg.ent_client].position     = vec3_init(0.0f, 1.0f, 0.0f);
  cg.bg.motion[cg.ent_client]                 = (bg_motion_t) {0};
  
  cg.bg.capsule[cg.ent_client].radius         = 0.5f;
  cg.bg.capsule[cg.ent_client].height         = 1.0f;
  
  cg.bg.health[cg.ent_client].max             = 100;
  cg.bg.health[cg.ent_client].now             = cg.bg.health[cg.ent_client].max;
  
  cg.bg.model[cg.ent_client]                  = BG_MDL_SKULL;
  cg.bg.weapon[cg.ent_client]                 = BG_WEAPON_PISTOL;
  
  cg.bg.attack[cg.ent_client].active          = false;
  cg.bg.attack[cg.ent_client].next_attack1    = 0;
  cg.bg.attack[cg.ent_client].next_attack2    = 0;
  
  entity_t entity = edict_add_entity(&cg.edict, BGC_TRANSFORM | BGC_PARTICLE);
  cg.bg.transform[entity].position     = vec3_init(4.0f, 5.0f, 3.0f);
  cg.bg.particle[entity].now_time = 0;
  cg.bg.particle[entity].end_time = 500;
  
  cl_load_map("nk_chito");
}


void cl_console()
{
  // Read a line from 'sys_in'
  const char *in_cmd = sys_read_in();
  
  if (in_cmd) {
    // The start of commands are denoted by '!' eg. '!score'
    if (in_cmd[0] == '!') {
      // If they are commands, execute them as one
      cmd_puts(&in_cmd[1]);
      cmd_puts("\n");
      cmd_execute();
    } else {
      // Otherwise they should be sent as a chat message
      cl_send_chat(in_cmd);
    }
  }
}

// The time until the next command to be sent
static int  next_cmd = 0;

void cl_update(int delta_time)
{
  // Build the usercmd
  in_base_move(&cl.usercmd);
  
  // Calculate the the amount of inteprolation
  float interp = ((float) next_cmd / (float) cl_cmdrate);
  
  // Updates to be done per frame
  cl_console();
  cl_view_look();
  cl_interpolate(interp);
  
  if (!cl.local)
    cl_parse();
  
  // Add the amount of time that has passed to the time until the next command
  next_cmd += delta_time;
  
  // When the amount of time since the last command sent has passed exceeds the
  // delay between the command to be sent the next command is ready to be processed
  if (next_cmd > cl_cmdrate) {
    // Modulo is used here in cases where the amount of time between each frame
    // is greater than cl_cmdrate. This happened when the program went out of
    // focus, the loop pausing until it was focused again leading to a huge
    // next_cmd. The loop then sent several usercmds without delay. The modulo
    // instead only processes the remaining time until the next delay. 
    next_cmd = next_cmd % cl_cmdrate;
    
    // Updates to be done per command sent
    // NOTE: because changes to the local game state are only relevant per
    // command sent, they should only be applied every new command
    
    if (cl.local) {
      cg.bg.client[cg.ent_client].usercmd = cl.usercmd;
      bg_update(&cg.bg);
    } else {
      cl_send_cmd();
      cl_predict();
    }
    
    cl_snapshot();
  }
  
  // Finally, render the client view after all updates applied
  r_render_client_view();
}
