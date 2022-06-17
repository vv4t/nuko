#include "log.h"

void log_printf(log_level_t level, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  
  const char *str_level;
  FILE *out;
  
  switch (level) {
  case LOG_DEBUG:
    str_level = "DEBUG";
    out = stdout;
    break;
  case LOG_WARNING:
    str_level = "WARNING";
    out = stdout;
    break;
  case LOG_ERROR:
    str_level = "ERROR";
    out = stderr;
    break;
  case LOG_FATAL:
    str_level = "FATAL";
    out = stderr;
    break;
  }
  
  fprintf(out, "[%s] ", str_level);
  vfprintf(out, fmt, args);
  fprintf(out, "\n");
  
  va_end(args);
  
  // close program if error is fatal
  if (level == LOG_FATAL)
    exit(-1);
}
