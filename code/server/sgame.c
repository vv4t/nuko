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

void sg_set_cmd(sgame_t *sg, entity_t entity, const usercmd_t *usercmd)
{
  sg->bg.client[entity].usercmd = *usercmd;
}

#define SG_CLIENT_SNAPSHOT_MASK (BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_PMOVE | BGC_MODEL)
void sg_client_snapshot(snapshot_t *snapshot, entity_t entity, const sgame_t *sg)
{
  memcpy(&snapshot->cl_pmove, &sg->bg.pmove[entity], sizeof(bg_pmove_t));
  memcpy(&snapshot->cl_motion, &sg->bg.motion[entity], sizeof(bg_motion_t));
  
  snapshot->cl_entity = sg->edict.entities[entity] & SG_CLIENT_SNAPSHOT_MASK;
}

#define SG_SERVER_SNAPSHOT_MASK (BGC_TRANSFORM | BGC_CAPSULE | BGC_MODEL)
void sg_server_snapshot(snapshot_t *snapshot, const sgame_t *sg)
{
  memcpy(&snapshot->edict, &sg->edict, sizeof(edict_t));
  memcpy(&snapshot->sv_transform, &sg->bg.transform, sizeof(sg->bg.transform));
  memcpy(&snapshot->sv_capsule, &sg->bg.capsule, sizeof(sg->bg.capsule));
  
  for (int i = 0; i < snapshot->edict.num_entities; i++)
    snapshot->edict.entities[i] &= SG_SERVER_SNAPSHOT_MASK;
}

entity_t sg_new_client(sgame_t *sg)
{
  entity_t entity = edict_add_entity(&sg->edict, SG_ES_SV_CLIENT);
  sg->bg.transform[entity] = (bg_transform_t) {0};
  sg->bg.motion[entity] = (bg_motion_t) {0};
  sg->bg.capsule[entity].radius = 0.5f;
  sg->bg.capsule[entity].height = 1.0f;
  sg->bg.transform[entity].position.y = 3;
  sg->bg.model[entity] = BG_MDL_FUMO_CIRNO;
  
  return entity;
}

void sg_remove_client(sgame_t *sg, entity_t entity)
{
  edict_remove_entity(&sg->edict, entity);
}
