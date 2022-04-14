#ifndef CGAME_H
#define CGAME_H

#include "../game/bsp.h"
#include "../game/map-file.h"
#include "../game/edict.h"
#include "../game/bgame.h"

typedef enum {
  CGES_PLAYER = BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_PMOVE
} cg_entitystate_t;

typedef struct {
  edict_t         edict;
  bgame_t         bgame;
  
  entity_t        entity_player;
} cgame_t;

void cgame_init(cgame_t *cgame);
void cgame_send_cmd(cgame_t *cgame, const usercmd_t *usercmd);
void cgame_new_map(cgame_t *cgame, const map_t *map);
void cgame_update(cgame_t *cgame);

#endif
