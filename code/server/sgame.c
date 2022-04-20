#include "sgame.h"

#include <string.h>

void sg_init(sgame_t *sg)
{
  edict_init(&sg->edict);
  bg_init(&sg->bg, &sg->edict);
}

void sg_new_map(sgame_t *sg, const map_t *map)
{
  bg_new_map(&sg->bg, map);
} 

void sg_update(sgame_t *sg)
{
  bg_update(&sg->bg);
}

#define SG_SNAPSHOT_MASK (BGC_TRANSFORM | BGC_CAPSULE | BGC_CLIP | BGC_MODEL)
void sg_build_snapshot(snapshot_t *snapshot, const sgame_t *sg)
{
  memcpy(&snapshot->edict, &sg->edict, sizeof(edict_t));
  
  memcpy(&snapshot->sv_transform, sg->bg.transform, sizeof(sg->bg.transform));
  memcpy(&snapshot->sv_capsule, sg->bg.capsule, sizeof(sg->bg.client));
  
  for (int i = 0; i < snapshot->edict.num_entities; i++)
    snapshot->edict.entities[i] &= SG_SNAPSHOT_MASK;
}

entity_t sg_add_client(sgame_t *sg)
{
  entity_t entity = edict_add_entity(&sg->edict, SG_ES_SV_CLIENT);
  sg->bg.capsule[entity].radius = 0.5f;
  sg->bg.capsule[entity].height = 1.0f;
  sg->bg.transform[entity].position.y = 3;
  sg->bg.model[entity] = BG_MDL_FUMO_CIRNO;
  
  return entity;
}
