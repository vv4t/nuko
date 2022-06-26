#include "cl_local.h"

#define CL_VIEW_LOOK (BGC_CLIENT | BGC_TRANSFORM)
void cl_view_look()
{
  if ((cg.edict.entities[cg.ent_client] & CL_VIEW_LOOK) != CL_VIEW_LOOK)
    return;
  
  pm_free_look(&cg.bg.transform[cg.ent_client], cl.usercmd.yaw, cl.usercmd.pitch);
}

void cl_predict()
{
  // Synchronise it with the last game state received from the server
  cl_reconcile();
  
  // If the client entity doesn't have a "client" component - isn't storing any
  // usercmds - don't predict anything
  // NOTE: this might be problematic if there are things which need to be
  // predicted unrelated to the client e.g. physics objects
  if ((cg.edict.entities[cg.ent_client] & BGC_CLIENT) != BGC_CLIENT)
    return;
  
  // From there, simulate the game for every usercmd which has yet to be
  // acknolwedged
  for (int i = cl.cmd_tail + 1; i < cl.cmd_head; i++) {
    cg.bg.client[cg.ent_client].usercmd = cl.cmd_queue[i % MAX_CMD_QUEUE];
    bg_update(&cg.bg);
  }
}

// This involves copying all components over from 'cg.snapshot' - the
// specifications of which are in bg_snapshot_t (game/bgame.h) - into the
// current client game. NOTE: all members with the prefix 'cl_' should be copied
// over to the local client while those prefixed by 'sv_' should overwrite all
// the components.
void cl_reconcile()
{
  cg.round_time = cl.snapshot.round_time;
  
  memcpy(&cg.edict, &cl.snapshot.edict, sizeof(edict_t));
  memcpy(&cg.bg.pmove[cg.ent_client], &cl.snapshot.cl_pmove, sizeof(cl.snapshot.cl_pmove));
  memcpy(&cg.bg.motion[cg.ent_client], &cl.snapshot.cl_motion, sizeof(cl.snapshot.cl_motion));
  memcpy(&cg.bg.health[cg.ent_client], &cl.snapshot.cl_health, sizeof(cl.snapshot.cl_health));
  
  memcpy(&cg.bg.model, &cl.snapshot.sv_model, sizeof(cl.snapshot.sv_model));
  memcpy(&cg.bg.transform, &cl.snapshot.sv_transform, sizeof(cl.snapshot.sv_transform));
  memcpy(&cg.bg.capsule, &cl.snapshot.sv_capsule, sizeof(cl.snapshot.sv_capsule));
  memcpy(&cg.bg.attack, &cl.snapshot.sv_attack, sizeof(cl.snapshot.sv_attack));
  memcpy(&cg.bg.weapon, &cl.snapshot.sv_weapon, sizeof(cl.snapshot.sv_weapon));
  memcpy(&cg.bg.particle, &cl.snapshot.sv_particle, sizeof(cl.snapshot.sv_particle));
  
  cg.edict.entities[cg.ent_client] = cl.snapshot.cl_entity_state;
}

void cl_snapshot()
{
  // Shift the latest snapshot "back"
  memcpy(&cg.from, &cg.to, sizeof(cl_snapshot_t));
  
  // Snapshot the new game state
  memcpy(&cg.to.transform, &cg.bg.transform, sizeof(cg.bg.transform));
  memcpy(&cg.to.attack, &cg.bg.attack, sizeof(cg.bg.attack));
  memcpy(&cg.to.particle, &cg.bg.particle, sizeof(cg.bg.particle));
}

// Interpolation reads from 'cg.to' and 'cg.from'. Any components to be
// interpolated should be put in the 'cl_snapshot' structure. NOTE: if this is
// done it should also be updated in cl_snapshot().
void cl_interpolate(float interp)
{
  // Interpolate using linear interpolation
  // tween = from + (to - from) * interp
  for (int i = 0; i < cg.edict.num_entities; i++) {
    // Interpolate entity transforms
    if ((cg.edict.entities[i] & BGC_TRANSFORM) == BGC_TRANSFORM) {
      vec3_t pos_from = cg.from.transform[i].position;
      vec3_t pos_to = cg.to.transform[i].position;
      vec3_t delta_pos = vec3_sub(pos_to, pos_from);
      vec3_t pos_tween = vec3_add(pos_from, vec3_mulf(delta_pos, interp));
      
      cg.tween.transform[i].position = pos_tween;
    }
    
    // Interpolate entity attacks
    if ((cg.edict.entities[i] & BGC_ATTACK) == BGC_ATTACK) {
      int next_attack1_from = cg.from.attack[i].next_attack1;
      int next_attack1_to = cg.to.attack[i].next_attack1;
      int delta_attack1 = next_attack1_to - next_attack1_from;
      
      int next_attack1_tween = next_attack1_from + (int) ((float) delta_attack1 * interp);
      
      cg.tween.attack[i].next_attack1 = next_attack1_tween;
    }
    
    // Interpolate entity attacks
    if ((cg.edict.entities[i] & BGC_PARTICLE) == BGC_PARTICLE) {
      int now_time_from = cg.from.particle[i].now_time;
      int now_time_to = cg.to.particle[i].now_time;
      int delta_now_time = now_time_to - now_time_from;
      
      int now_time_tween = now_time_from + (int) ((float) delta_now_time * interp);
      
      cg.tween.particle[i].now_time = now_time_tween;
    }
  }
}

void cl_load_map(const char *name)
{
  char map_path[256];
  snprintf(map_path, sizeof(map_path), "assets/map/%s.map", name);
  
  map_t map;
  
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "cl_load_map(): failed to load %s", map_path);
  
  if (!r_new_map(&map))
    log_printf(LOG_FATAL, "cl_load_map(): renderer failed to load new map");
  
  bg_new_map(&cg.bg, &map);
}
