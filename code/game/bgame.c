#include "bg-local.h"

void bgame_init(bgame_t *bgame, edict_t *edict)
{
  bgame->edict = edict;
  bgame->bsp = NULL;
}

void bgame_new_map(bgame_t *bgame, const map_t *map)
{
  if (bgame->bsp)
    map_free_bsp(bgame->bsp);
  
  bgame->bsp = map_load_bsp(map);
}

void bgame_update(bgame_t *bgame)
{
  bgame_motion_gravity(bgame);
  bgame_player_look(bgame);
  bgame_player_move(bgame);
  bgame_motion_integrate(bgame);
  bgame_clip_capsule_bsp(bgame);
  bgame_motion_clip(bgame);
  bgame_player_test_grounded(bgame);
}
