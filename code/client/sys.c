#include "sys.h"

#include "../common/cmd.h"
#include "../common/log.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_EVENTS    32
#define MAX_KEYBINDS  32

typedef struct {
  int         key;
  const char  *text;
} keybind_t;

static keybind_t    sys_keybinds[MAX_KEYBINDS];
static int          sys_num_keybinds;

static sys_event_t  sys_event_queue[MAX_EVENTS];
static int          sys_event_tail;
static int          sys_event_head;

void sys_init()
{
  sys_num_keybinds = 0;
  
  sys_event_head = 0;
  sys_event_tail = 0;
}

void sys_bind(const char *text, int key)
{
  if (sys_num_keybinds + 1 > MAX_KEYBINDS) {
    log_printf(LOG_ERROR, "sys_bind(): ran out of bindings");
    return;
  }
  
  sys_keybinds[sys_num_keybinds].key = key;
  sys_keybinds[sys_num_keybinds].text = text;
  sys_num_keybinds++;
}

void sys_key_press(int key, int action)
{
  for (int i = 0; i < sys_num_keybinds; i++) {
    if (sys_keybinds[i].key == key) {
      if (sys_keybinds[i].text[0] == '+') {
        cmd_puts(action ? "+" : "-");
        cmd_puts(&sys_keybinds[i].text[1]);
      } else if (action) {
        cmd_puts(sys_keybinds[i].text);
      }
      cmd_puts("\n");
    }
  }
  
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
