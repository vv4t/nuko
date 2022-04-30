#ifndef SGAME_H
#define SGAME_H

#include "../game/bgame.h"
#include "../game/map_file.h"

typedef enum {
  SG_ES_SV_CLIENT = BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_PMOVE | BGC_MODEL | BGC_SHOOT
} sg_entitystate_t;

typedef struct {
  edict_t   edict;
  bgame_t   bg;
} sgame_t;

void      sg_init(sgame_t *sg);
void      sg_update(sgame_t *sg);
void      sg_new_map(sgame_t *sg, const map_t *map);

entity_t  sg_new_client(sgame_t *sg);
void      sg_remove_client(sgame_t *sg, entity_t entity);
void      sg_set_cmd(sgame_t *sg, entity_t entity, const usercmd_t *usercmd);

void      sg_server_snapshot(snapshot_t *snapshot, const sgame_t *sg);
void      sg_client_snapshot(snapshot_t *snapshot, entity_t entity, const sgame_t *sg);

#endif
