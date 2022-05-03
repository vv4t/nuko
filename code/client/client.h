#ifndef CLIENT_H
#define CLIENT_H

#include "../game/edict.h"
#include "../game/bgame.h"

typedef struct {
  bg_transform_t  transform[MAX_ENTITIES];
} cl_snapshot_t;

typedef struct {
  edict_t         edict;
  bgame_t         bg;
  
  cl_snapshot_t   to;
  cl_snapshot_t   from;
  cl_snapshot_t   tween;
  
  entity_t        ent_client;
} cgame_t;

extern cgame_t cg;

void cl_init();
void cl_update(int delta_time);

#endif
