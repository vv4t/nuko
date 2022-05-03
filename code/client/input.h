#ifndef INPUT_H
#define INPUT_H

#include "../game/bgame.h"
#include "../common/string.h"
#include "../common/log.h"
#include "../common/cmd.h"
#include <stdbool.h>

#define IN_SENSITIVITY  0.005

//
// input.c
//
void in_init();
void in_mouse_move(int dx, int dy);
void in_mouse_event(int button, int action);
void in_key_event(int key, int action);
void in_key_bind(int key, const char *text);
void in_base_move(usercmd_t *cmd);

#endif
