#include "cmd.h"

#define MAX_TEXT        256 // Command buffer size
#define MAX_ARGS        8   // Maximum arguments
#define MAX_FUNCTIONS   32  // Maximum command functions

typedef struct {
  const char  *name;
  void        *d; // Context data
  xcommand_t  xcommand;
} cmd_function_t;

static char            cmd_text[MAX_TEXT]           = {0};
static char            *cmd_text_ptr                = cmd_text;

static char            *cmd_arg_value[MAX_ARGS]     = {0};
static int             cmd_arg_count                = 0;

static cmd_function_t  cmd_functions[MAX_FUNCTIONS] = {0};
static int             cmd_num_functions            = 0;

bool cmd_execute()
{
  // Do nothing if command buffer is empty
  if (cmd_text_ptr == cmd_text)
    return true;
  
  char *cmd_queue = cmd_text;
  
  // VERY CONFUSING strtok_r CODE
  
  char *str_cmd;
  while ((str_cmd = strtok_r(cmd_queue, "\n", &cmd_queue))) { // Split multiple commands by new line
    cmd_arg_count = 0;
    
    while ((cmd_arg_value[cmd_arg_count] = strtok_r(str_cmd, " ", &str_cmd))) { // Separate arguments
      cmd_arg_count++;
      // Account for arguments wrapped in ""
      while (*str_cmd == '"') {
        cmd_arg_value[cmd_arg_count++] = ++str_cmd;
        
        // Make sure the " is closed
        while (*str_cmd != '"') {
          if (*str_cmd == '\n' || *str_cmd == 0) {
            log_printf(LOG_ERROR, "cmd_execute(): unescaped '\"'");
            return false;
          }
          
          str_cmd++;
        }
        
        do
          *str_cmd++ = 0;
        while (*str_cmd == ' ');
      }
    }
    
    // Linear search for existing commands
    for (int i = 0; i < cmd_num_functions; i++) {
      if (strcmp(cmd_arg_value[0], cmd_functions[i].name) == 0) {
        if (cmd_functions[i].xcommand)
          cmd_functions[i].xcommand(cmd_functions[i].d);
        else
          log_printf(LOG_ERROR, "cmd_execute(): command '%s' is empty", cmd_functions[i].name);
        
        break;
      }
    }
  }
  
  cmd_text_ptr = cmd_text;
  
  return true;
}

void cmd_puts(const char *text)
{
  int len = strlen(text);
  
  if (cmd_text_ptr + len >= &cmd_text[MAX_TEXT]) { // Boundary checks
    log_printf(LOG_ERROR, "cmd_puts(): ran out of memory");
    return;
  }
  
  strcpy(cmd_text_ptr, text);
  cmd_text_ptr += len;
}

void cmd_add_command(const char *text, xcommand_t xcommand, void *d)
{
  const char *name = string_copy(text);
  
  if (!name) {
    log_printf(LOG_ERROR, "cmd_add_command(): could not allocate string");
    return;
  }
  
  if (cmd_num_functions + 1 >= MAX_FUNCTIONS) {
    log_printf(LOG_ERROR, "cmd_add_command(): ran out of memory");
    return;
  }
  
  // Create new entry in array
  cmd_function_t *cmd_function = &cmd_functions[cmd_num_functions++];
  cmd_function->name = name;
  cmd_function->xcommand = xcommand;
  cmd_function->d = d;
}

int cmd_argc()
{
  return cmd_arg_count;
}

const char *cmd_argv(int n)
{
  return cmd_arg_value[n];
}
