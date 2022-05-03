#ifndef CMD_H
#define CMD_H

#include "log.h"
#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void (*xcommand_t)(void *d);

bool        cmd_execute();
void        cmd_puts(const char *text);
void        cmd_bind(int key, const char *cmd);
void        cmd_add_command(const char *cmd, xcommand_t xcommand, void *d);
int         cmd_argc();
const char  *cmd_argv(int n);

#endif
