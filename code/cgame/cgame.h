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
  edict_t edict;
  
  bgc_transform_t bgc_transform[MAX_ENTITIES];
  bgc_client_t bgc_client[MAX_ENTITIES];
  bgc_capsule_t bgc_capsule[MAX_ENTITIES];
  bgc_clip_t bgc_clip[MAX_ENTITIES];
  bgc_motion_t bgc_motion[MAX_ENTITIES];
  bgc_pmove_t bgc_pmove[MAX_ENTITIES];
  
  bgame_t bg;
  
  entity_t entity_player;
} cgame_t;

void cg_init(cgame_t *cg);
void cg_send_cmd(cgame_t *cg, const usercmd_t *usercmd);
void cg_new_map(cgame_t *cg, const map_t *map);
void cg_update(cgame_t *cg);

#endif
