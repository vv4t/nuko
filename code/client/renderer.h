#ifndef RENDERER_H
#define RENDERER_H

bool r_init();
bool r_new_map(const map_t *map);
void r_render_player_view(const cgame_t *cg);

#endif
