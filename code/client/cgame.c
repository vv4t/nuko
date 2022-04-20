#include "cgame.h"

#include <string.h>

static usercmd_t usercmd_cache[MAX_USERCMD_CACHE];

void cg_init(cgame_t *cg)
{
  bg_init(&cg->bg, &cg->edict);
}

void cg_send_cmd(cgame_t *cg, const usercmd_t *usercmd)
{
  usercmd_cache[cg->outgoing_seq % MAX_USERCMD_CACHE] = *usercmd;
}

void cg_new_map(cgame_t *cg, const map_t *map)
{
  bg_new_map(&cg->bg, map);
}

void cg_update(cgame_t *cg)
{
  cg_reconcile(cg);
  
  for (int i = cg->incoming_ack + 1; i < cg->outgoing_seq; i++) {
    cg->bg.client[cg->player_entity].usercmd = usercmd_cache[i % MAX_USERCMD_CACHE];
    bg_update(&cg->bg);
  }
}

void cg_set_player(cgame_t *cg, entity_t player)
{
  cg->player_entity = player;
  cg->bg.capsule[cg->player_entity].radius = 0.5f;
  cg->bg.capsule[cg->player_entity].height = 1.0f;
  cg->incoming_ack = 0;
  cg->outgoing_seq = 0;
}

void cg_reconcile(cgame_t *cg)
{
  memcpy(&cg->edict, &cg->snapshot.edict, sizeof(edict_t));
  
  cg->bg.pmove[cg->player_entity] = cg->snapshot.cl_pmove;
  cg->bg.motion[cg->player_entity] = cg->snapshot.cl_motion;
  
  memcpy(&cg->bg.transform, &cg->snapshot.sv_transform, sizeof(cg->snapshot.sv_transform));
  memcpy(&cg->bg.capsule, &cg->snapshot.sv_capsule, sizeof(cg->snapshot.sv_capsule));
  
  cg->edict.entities[cg->player_entity] |= CG_ES_CL_CLIENT;
}
