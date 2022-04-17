#include "sys.h"

#include "../common/cmd.h"
#include "../common/log.h"
#include <stdio.h>

#define MAX_EVENTS    32
#define MAX_KEYBINDS  32

typedef struct {
  int         key;
  const char  *text;
} keybind_t;

static keybind_t    sys_keybinds[MAX_KEYBINDS];
static int          sys_num_keybinds = 0;

static sys_event_t  sys_event_queue[MAX_EVENTS];
static int          sys_event_tail = 0;
static int          sys_event_head = 0;

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

void sys_console_input(void *d)
{
  char text_buf[256];
  
  printf("> ");
  const char *text = fgets(text_buf, 256, stdin);
  
  cmd_puts(text);
}

void sys_console_key_press(int key, int action)
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
}

void sys_queue_event(const sys_event_t *sys_event)
{
  sys_event_queue[sys_event_head] = *sys_event;
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
