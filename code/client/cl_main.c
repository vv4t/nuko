#include "cl_local.h"

client_t    cl;
cgame_t     cg;

// Rate at which to send usercmds
// NOTE: althought it is named rate it is similar to host_frametime, in that
// it is the milliseconds between sending each usercmd.
static int  cl_cmdrate  = 50;

// Connect to the server
void connect_f(void *d)
{
  cl_connect();
}

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
  // Add client console commands
  cmd_add_command("say", say_f, NULL);
  cmd_add_command("name", name_f, NULL);
  cmd_add_command("score", score_f, NULL);
  cmd_add_command("connect", connect_f, NULL);
  
  // Reset usercmd cache
  cl.connection = CONN_DISCONNECTED;
  cl.cmd_tail = 0;
  cl.cmd_head = 0;
  
  // Initialize game state
  edict_init(&cg.edict);
  bg_init(&cg.bg, &cg.edict);
  
  // Initialize renderer
  if (!r_init())
    log_printf(LOG_FATAL, "cl_init(): failed to initialize renderer");
  
  cl_tutorial_init();
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
    
    if (cl.connection != CONN_CONNECTED) {
      cg.bg.client[cg.ent_client].usercmd = cl.usercmd;
      bg_update(&cg.bg);
      cl_tutorial_attack();
      cl_tutorial_dummy_death();
      cl_tutorial_round();
    } else {
      cl_send_cmd();
      cl_predict();
    }
    
    cl_snapshot();
  }
  
  // Finally, render the client view after all updates applied
  r_render_client_view();
}
