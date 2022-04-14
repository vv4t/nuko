#ifndef WIN_H
#define WIN_H

#include <stdbool.h>

bool win_init(int width, int height, const char *title);
bool win_should_quit();
void win_poll();
void win_swap();
void win_quit();
void win_cursor_lock(bool active);

#endif
