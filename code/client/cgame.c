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

void cg_set_ent_player(cgame_t *cg, entity_t ent_player)
{
  cg->ent_player = ent_player;
}

void cg_set_cmd(cgame_t *cg, const usercmd_t *usercmd)
{
  cg->bg.client[cg->ent_player].usercmd = *usercmd;
}

void cg_reconcile(cgame_t *cg, const snapshot_t *snapshot)
{
  memcpy(&cg->edict, &snapshot->edict, sizeof(edict_t));
  memcpy(&cg->bg.pmove[cg->ent_player], &snapshot->cl_pmove, sizeof(snapshot->cl_pmove));
  memcpy(&cg->bg.motion[cg->ent_player], &snapshot->cl_motion, sizeof(snapshot->cl_motion));
  memcpy(&cg->bg.transform, &snapshot->sv_transform, sizeof(snapshot->sv_transform));
  memcpy(&cg->bg.capsule, &snapshot->sv_capsule, sizeof(snapshot->sv_capsule));
  
  cg->edict.entities[cg->ent_player] = snapshot->cl_entity;
}
