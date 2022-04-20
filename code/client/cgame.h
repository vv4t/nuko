#ifndef CGAME_H
#define CGAME_H

#include "../game/bsp.h"
#include "../game/map_file.h"
#include "../game/edict.h"
#include "../game/bgame.h"

#define MAX_USERCMD_CACHE 1024

typedef enum {
  CG_ES_CL_CLIENT = BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_PMOVE | BGC_MODEL,
} cg_entitystate_t;

typedef struct {
  edict_t         edict;
  bgame_t         bg;
  
  snapshot_t      snapshot;
  
  int             incoming_ack;
  int             outgoing_seq;
  
  entity_t        player_entity;
} cgame_t;

void cg_init(cgame_t *cg);
void cg_send_cmd(cgame_t *cg, const usercmd_t *usercmd);
void cg_new_map(cgame_t *cg, const map_t *map);
void cg_set_player(cgame_t *cg, entity_t player);
void cg_reconcile(cgame_t *cg);
void cg_update(cgame_t *cg);

#endif
