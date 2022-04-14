#ifndef BGAME_H
#define BGAME_H

#include "bsp.h"
#include "edict.h"
#include "usercmd.h"
#include "map-file.h"
#include "../common/nk-math.h"
#include <stdbool.h>

#define MAX_CLIP_PLANES 16

typedef enum {
  BGC_TRANSFORM = (1 << 0),
  BGC_CLIENT    = (1 << 1),
  BGC_CLIP      = (1 << 2),
  BGC_CAPSULE   = (1 << 3),
  BGC_MOTION    = (1 << 4),
  BGC_PMOVE     = (1 << 5)
} bg_component_t;

typedef struct {
  vec3_t    position;
  quat_t    rotation;
} bg_transform_t;

typedef struct {
  usercmd_t usercmd;
} bg_client_t;

typedef struct {
  float     radius;
  float     height;
} bg_capsule_t;

typedef struct {
  plane_t   planes[MAX_CLIP_PLANES];
  int       num_planes;
} bg_clip_t;

typedef struct {
  vec3_t    velocity;
} bg_motion_t;

typedef struct {
  quat_t    move_rot;
  bool      is_grounded;
} bg_pmove_t;

typedef struct {
  edict_t        *edict;
  bsp_node_t     *bsp;
  
  bg_transform_t transform[MAX_ENTITIES];
  bg_client_t    client[MAX_ENTITIES];
  bg_clip_t      clip[MAX_ENTITIES];
  bg_capsule_t   capsule[MAX_ENTITIES];
  bg_motion_t    motion[MAX_ENTITIES];
  bg_pmove_t     pmove[MAX_ENTITIES];
} bgame_t;

void bgame_init(bgame_t *bgame, edict_t *edict);
void bgame_new_map(bgame_t *bgame, const map_t *map);
void bgame_update(bgame_t *bgame);

#endif
