#include "cgame.h"

#include <string.h>

void cg_init(cgame_t *cg)
{
  bg_init(&cg->bg, &cg->edict);
}

void cg_new_map(cgame_t *cg, const map_t *map)
{
  bg_new_map(&cg->bg, map);
}

void cg_update(cgame_t *cg)
{
  bg_update(&cg->bg);
}

void cg_set_player(cgame_t *cg, entity_t player)
{
  cg->player_entity = player;
  cg->bg.capsule[cg->player_entity].radius = 0.5f;
  cg->bg.capsule[cg->player_entity].height = 1.0f;
}

void cg_set_cmd(cgame_t *cg, const usercmd_t *usercmd)
{
  cg->bg.client[cg->player_entity].usercmd = *usercmd;
}

void cg_reconcile(cgame_t *cg, const snapshot_t *snapshot)
{
  memcpy(&cg->edict, &snapshot->edict, sizeof(edict_t));
  
  cg->bg.pmove[cg->player_entity] = snapshot->cl_pmove;
  cg->bg.motion[cg->player_entity] = snapshot->cl_motion;
  
  memcpy(&cg->bg.transform, &snapshot->sv_transform, sizeof(snapshot->sv_transform));
  memcpy(&cg->bg.capsule, &snapshot->sv_capsule, sizeof(snapshot->sv_capsule));
  
  cg->edict.entities[cg->player_entity] |= CG_ES_CL_CLIENT;
}
