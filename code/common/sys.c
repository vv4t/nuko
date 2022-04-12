#include "sys.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_EVENTS 8

static sys_event_t sys_event_queue[MAX_EVENTS];
static int sys_event_tail;
static int sys_event_head;

void sys_init()
{
  sys_event_head = 0;
  sys_event_tail = 0;
}

void *sys_malloc(int size)
{
  return malloc(size);
}

void sys_free(void *block)
{
  free(block);
}

void sys_key_press(int key, int action)
{
  sys_event_queue[sys_event_head].type = SYS_KEY_PRESS;
  sys_event_queue[sys_event_head].data.key_press.key = key;
  sys_event_queue[sys_event_head].data.key_press.action = action;
  
  sys_event_head = (sys_event_head + 1) % MAX_EVENTS;
}

void sys_mouse_move(int dx, int dy)
{
  sys_event_queue[sys_event_head].type = SYS_MOUSE_MOVE;
  sys_event_queue[sys_event_head].data.mouse_move.dx = dx;
  sys_event_queue[sys_event_head].data.mouse_move.dy = dy;
  
  sys_event_head = (sys_event_head + 1) % MAX_EVENTS;
}

sys_event_t *sys_get_event()
{
  if (sys_event_tail != sys_event_head) {
    int current_event = sys_event_tail;
    sys_event_tail = (sys_event_tail + 1) % MAX_EVENTS;
    
    return &sys_event_queue[current_event];
  }
  
  return NULL;
}

void sys_log(sys_level_t level, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  
  const char *str_level;
  FILE *out;
  
  switch (level) {
  case SYS_DEBUG:
    str_level = "DEBUG";
    out = stdout;
    break;
  case SYS_WARNING:
    str_level = "WARNING";
    out = stdout;
    break;
  case SYS_ERROR:
    str_level = "ERROR";
    out = stderr;
    break;
  case SYS_FATAL:
    str_level = "FATAL";
    out = stderr;
    break;
  }
  
  fprintf(out, "[%s] ", str_level);
  vfprintf(out, fmt, args);
  fprintf(out, "\n");
  
  va_end(args);
  
  if (level == SYS_FATAL)
    exit(-1);
}
