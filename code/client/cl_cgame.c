#include "cl_local.h"

#define CL_VIEW_LOOK (BGC_CLIENT | BGC_TRANSFORM)
void cl_view_look()
{
  if ((cg.edict.entities[cg.ent_client] & CL_VIEW_LOOK) != CL_VIEW_LOOK)
    return;
  
  pm_free_look(&cg.bg.transform[cg.ent_client], cl.usercmd.yaw, cl.usercmd.pitch);
}

#define CL_PREDICT (BGC_CLIENT)
void cl_predict()
{
  cl_reconcile();
  
  if ((cg.edict.entities[cg.ent_client] & CL_PREDICT) != CL_PREDICT)
    return;
  
  for (int i = cl.cmd_tail + 1; i < cl.cmd_head; i++) {
    cg.bg.client[cg.ent_client].usercmd = cl.cmd_queue[i % MAX_CMD_QUEUE];
    bg_update(&cg.bg);
  }
}

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
  
  cg.edict.entities[cg.ent_client] = cl.snapshot.cl_entity_state;
}

void cl_snapshot()
{
  memcpy(&cg.from, &cg.to, sizeof(cl_snapshot_t));
  memcpy(&cg.to.transform, &cg.bg.transform, sizeof(cg.bg.transform));
  memcpy(&cg.to.attack, &cg.bg.attack, sizeof(cg.bg.attack));
}

void cl_interpolate(float interp)
{
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & BGC_TRANSFORM) == BGC_TRANSFORM) {
      vec3_t pos_from = cg.from.transform[i].position;
      vec3_t pos_to = cg.to.transform[i].position;
      vec3_t delta_pos = vec3_sub(pos_to, pos_from);
      
      vec3_t pos_tween = vec3_add(pos_from, vec3_mulf(delta_pos, interp));
      
      cg.tween.transform[i].position = pos_tween;
    }
    
    if ((cg.edict.entities[i] & BGC_ATTACK) == BGC_ATTACK) {
      int next_attack_from = cg.from.attack[i].next_attack;
      int next_attack_to = cg.to.attack[i].next_attack;
      int delta_attack = next_attack_to - next_attack_from;
      
      int next_attack_tween = next_attack_from + (int) ((float) delta_attack * interp);
      
      cg.tween.attack[i].next_attack = next_attack_tween;
    }
  }
}

void cl_load_map(const char *name)
{
  char map_path[256];
  sprintf(map_path, "assets/map/%s.map", name);
  
  map_t map;
  
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "cl_load_map(): failed to load %s", map_path);
  
  if (!r_new_map(&map))
    log_printf(LOG_FATAL, "cl_load_map(): renderer failed to load new map");
  
  bg_new_map(&cg.bg, &map);
}
