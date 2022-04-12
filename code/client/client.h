#ifndef CLIENT_H
#define CLIENT_H

#include "../common/usercmd.h"

void cl_event_loop();
void cl_key_press(int key, int action);
void cl_mouse_move(int dx, int dy);
void cl_base_move(usercmd_t *usercmd);

#endif
