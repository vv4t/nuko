#ifndef SYS_H
#define SYS_H

typedef enum {
  SYS_KEY_PRESS,
  SYS_MOUSE_MOVE
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

void        sys_bind(const char *text, int key);
void        sys_console_input();

void        sys_key_press(int key, int action);
void        sys_mouse_move(int dx, int dy);

sys_event_t *sys_get_event();

#endif
