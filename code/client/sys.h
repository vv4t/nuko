#ifndef SYS_H
#define SYS_H

#include <stdbool.h>

typedef enum {
  SYS_KEY_PRESS,
  SYS_MOUSE_MOVE,
  SYS_QUIT
} sys_evt_t;

typedef struct {
  sys_evt_t type;
  union {
    struct {
      int   key;
      int   action;
    } key_press;
    struct {
      int   dx;
      int   dy;
    } mouse_move;
  } data;
} sys_event_t;

void        sys_init();
void        sys_quit();

bool        sys_win_init(int width, int height, const char *title);
void        sys_win_swap();
void        sys_win_poll();
void        sys_win_quit();

void        sys_bind(const char *text, int key);
void        sys_console_input();
void        sys_console_key_press(int key, int action);
void        sys_queue_event(const sys_event_t *sys_event);
sys_event_t *sys_get_event();

#endif
