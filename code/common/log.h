/*
-- log.h --

Formatted logging function with minor utilities
*/

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// FATAL will cause the program to exit
typedef enum {
  LOG_DEBUG,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FATAL
} log_level_t;

void log_printf(log_level_t level, const char *fmt, ...);

#endif
