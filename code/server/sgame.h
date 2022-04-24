#ifndef SGAME_H
#define SGAME_H

#include "../game/bgame.h"
#include "../game/map_file.h"

typedef enum {
  SG_ES_SV_CLIENT = BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_PMOVE | BGC_MODEL
} sg_entitystate_t;

typedef struct {
  edict_t   edict;
  bgame_t   bg;
} sgame_t;

void      sg_init(sgame_t *sg);
void      sg_new_map(sgame_t *sg, const map_t *map);
entity_t  sg_add_client(sgame_t *sg);
void      sg_remove_client(sgame_t *sg, entity_t entity);
void      sg_build_snapshot(snapshot_t *snapshot, const sgame_t *sg);
void      sg_update(sgame_t *sg);

#endif
