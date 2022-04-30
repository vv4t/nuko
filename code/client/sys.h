#ifndef SYS_H
#define SYS_H

#include <stdbool.h>

#define     NUKO_WIDTH   960
#define     NUKO_HEIGHT  540
#define     NUKO_TITLE   "nuko"

void        sys_init();
void        sys_config();
bool        sys_win_init();
void        sys_poll();
void        sys_swap();
void        sys_quit();
int         sys_time();

#endif
