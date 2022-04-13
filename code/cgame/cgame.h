#ifndef CGAME_H
#define CGAME_H

#include "../common/bsp.h"
#include "../common/map-file.h"
#include "../common/edict.h"
#include "../game/bgame.h"

typedef enum {
  CGES_PLAYER = BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_PMOVE
} cg_entitystate_t;

typedef struct {
  edict_t         edict;
  bgame_t         bgame;
  
  bg_transform_t  bg_transform[MAX_ENTITIES];
  bg_client_t     bg_client[MAX_ENTITIES];
  bg_capsule_t    bg_capsule[MAX_ENTITIES];
  bg_clip_t       bg_clip[MAX_ENTITIES];
  bg_motion_t     bg_motion[MAX_ENTITIES];
  bg_pmove_t      bg_pmove[MAX_ENTITIES];
  
  entity_t        entity_player;
} cgame_t;

void cgame_init(cgame_t *cgame);
void cgame_send_cmd(cgame_t *cgame, const usercmd_t *usercmd);
void cgame_new_map(cgame_t *cgame, const map_t *map);
void cgame_update(cgame_t *cgame);

#endif
