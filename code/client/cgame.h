#ifndef CGAME_H
#define CGAME_H

#include "../game/bsp.h"
#include "../game/map_file.h"
#include "../game/edict.h"
#include "../game/bgame.h"

typedef struct {
  edict_t         edict;
  bgame_t         bg;
  
  entity_t        ent_player;
} cgame_t;

void cg_init(cgame_t *cg);
void cg_new_map(cgame_t *cg, const map_t *map);
void cg_reconcile(cgame_t *cg, const snapshot_t *snapshot);
void cg_update(cgame_t *cg);

void cg_set_cmd(cgame_t *cg, const usercmd_t *usercmd);
void cg_set_ent_player(cgame_t *cg, entity_t ent_player);

#endif
