#include "bg_local.h"

void bg_init(bgame_t *bg, edict_t *edict)
{
  bg->edict = edict;
  bg->bsp = NULL;
}

void bg_new_map(bgame_t *bg, const map_t *map)
{
  if (bg->bsp) // Free the old BSP if it exists
    map_free_bsp(bg->bsp);
  
  bg->bsp = map_load_bsp(map);
}

void bg_update(bgame_t *bg)
{
  // Run all systems
  // NOTE: order is important
  
  bg_pm_attack(bg);
  bg_pm_free_look(bg);
  bg_pm_walk_move(bg);
  bg_motion_gravity(bg);
  bg_motion_integrate(bg);
  bg_clip_capsule_bsp(bg);
  bg_motion_clip(bg);
  bg_pm_check_pos(bg);
  bg_pm_check_weapon(bg);
  bg_particle_tick(bg);
}
