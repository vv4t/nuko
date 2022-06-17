/*
-- cmd.h --

Reads commands from a buffer and executes them.
Commands are in the format: [alias] [args1] [arg2] ... [command separator]
Command separators: '\n', '\x00', ';'
*/

#ifndef CMD_H
#define CMD_H

#include "log.h"
#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// A command is call back function given a string alias.
// void *d used for pass context if necessary
typedef void (*xcommand_t)(void *d);

// Execute and clear the command buffer
bool        cmd_execute();

// Append to buffer
void        cmd_puts(const char *text);

// Add a command
void        cmd_add_command(const char *cmd, xcommand_t xcommand, void *d);

// Get the number of arguments
int         cmd_argc();

// Get 'nth' argument starting from 1. cmd_argv(0) is reserved for command name
const char  *cmd_argv(int n);

#endif
