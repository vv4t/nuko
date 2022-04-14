#include "cmd.h"

#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TEXT        256
#define MAX_STR         256
#define MAX_ARGS        8
#define MAX_FUNCTIONS   32

typedef struct {
  const char  *name;
  void        *d;
  xcommand_t  xcommand;
} cmd_function_t;

typedef struct {
  const char  *text;
} cmd_bind_t;

typedef struct {
  char            text[MAX_TEXT];
  char            *text_ptr;
  
  char            str[MAX_STR];
  char            *str_ptr;
  
  int             argc;
  char            *argv[MAX_ARGS];
  
  cmd_function_t  functions[MAX_FUNCTIONS];
  int             num_functions;
} cmd_t;

static cmd_t cmd;

void cmd_init()
{
  cmd.text_ptr = cmd.text;
  cmd.str_ptr = cmd.str;
  cmd.num_functions = 0;
  
  memset(cmd.str, 0, sizeof(cmd.str));
  memset(cmd.text, 0, sizeof(cmd.text));
}

static char *str_new(const char *str)
{
  char *new_str = cmd.str_ptr;
  int len = strlen(str) + 1;
  
  if (cmd.str_ptr + len >= &cmd.str[MAX_STR]) {
    log_printf(LOG_ERROR, "str_new(): ran out of memory");
    return NULL;
  }
  
  cmd.str_ptr += strlen(str) + 1;
  strcpy(new_str, str);
  
  return new_str;
}

void cmd_execute()
{
  if (cmd.text_ptr == cmd.text)
    return;
  
  char *cmd_queue = cmd.text;
  
  char *str_cmd;
  while ((str_cmd = strtok_r(cmd_queue, ";\n", &cmd_queue))) {
    cmd.argc = 0;
    
    while ((cmd.argv[cmd.argc] = strtok_r(str_cmd, " ", &str_cmd)))
      cmd.argc++;
    
    for (int i = 0; i < cmd.num_functions; i++) {
      if (strcmp(cmd.argv[0], cmd.functions[i].name) == 0) {
        cmd.functions[i].xcommand(cmd.functions[i].d);
        break;
      }
    }
  }
  
  cmd.text_ptr = cmd.text;
}

void cmd_puts(const char *text)
{
  int len = strlen(text);
  
  if (cmd.text_ptr + len >= &cmd.text[MAX_TEXT]) {
    log_printf(LOG_ERROR, "cmd_puts(): ran out of memory");
    return;
  }
  
  strcpy(cmd.text_ptr, text);
  cmd.text_ptr += len;
}

void cmd_add_command(const char *text, xcommand_t xcommand, void *d)
{
  const char *name = str_new(text);
  
  if (!name) {
    log_printf(LOG_ERROR, "cmd_add_command(): could not allocate string");
    return;
  }
  
  if (cmd.num_functions + 1 >= MAX_FUNCTIONS) {
    log_printf(LOG_ERROR, "cmd_add_command(): ran out of memory");
    return;
  }
  
  cmd_function_t *cmd_function = &cmd.functions[cmd.num_functions++];
  cmd_function->name = name;
  cmd_function->xcommand = xcommand;
  cmd_function->d = d;
}

int cmd_argc()
{
  return cmd.argc;
}

const char *cmd_argv(int n)
{
  return cmd.argv[n];
}
