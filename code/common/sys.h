#ifndef SYS_H
#define SYS_H

typedef enum {
  SYS_DEBUG,
  SYS_WARNING,
  SYS_ERROR,
  SYS_FATAL
} sys_level_t;

typedef enum {
  SYS_KEY_PRESS,
  SYS_MOUSE_MOVE
} sys_evt_t;

typedef struct {
  sys_evt_t type;
  union {
    struct {
      int key;
      int action;
    } key_press;
    struct {
      int dx;
      int dy;
    } mouse_move;
  } data;
} sys_event_t;

void *sys_malloc(int size);
void sys_free(void *block);

void sys_log(sys_level_t level, const char *fmt, ...);

void sys_key_press(int key, int action);
void sys_mouse_move(int dx, int dy);
sys_event_t *sys_get_event();

#define FLAG() (sys_log(SYS_DEBUG, "HERE"))

#endif
