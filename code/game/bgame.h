#ifndef BGAME_H
#define BGAME_H

#include "../common/bsp.h"
#include "../common/edict.h"
#include "../common/nk-math.h"
#include "../common/usercmd.h"
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
  
  bg_transform_t *transform;
  bg_client_t    *client;
  bg_clip_t      *clip;
  bg_capsule_t   *capsule;
  bg_motion_t    *motion;
  bg_pmove_t     *pmove;
} bgame_t;

void bgame_update(bgame_t *bgame);

#endif
