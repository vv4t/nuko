/*
--renderer.h (r) --

Public definition of renderer modules.

The renderer creates a visual representation for the client based off cgame_t
from client.h.
*/
#ifndef RENDERER_H
#define RENDERER_H

#include "../game/map_file.h"
#include <stdbool.h>

bool r_init();
bool r_new_map(const map_t *map);
void r_render_client_view();

#endif
