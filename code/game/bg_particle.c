#include "bg_local.h"

#define BG_PARTICLE_TICK (BGC_PARTICLE)
void bg_particle_tick(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_PARTICLE_TICK) != BG_PARTICLE_TICK)
      continue;
    
    bg->particle[i].now_time += BG_TIMESTEP * 1000;
  }
}
