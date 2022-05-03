#ifndef RENDERER_H
#define RENDERER_H

#include "../game/map_file.h"
#include <stdbool.h>

bool r_init();
bool r_new_map(const map_t *map);
void r_render_client_view();

#endif
