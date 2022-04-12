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
} bgc_t;

typedef struct {
  vec3_t pos;
  quat_t rot;
} bgc_transform_t;

typedef struct {
  usercmd_t usercmd;
} bgc_client_t;

typedef struct {
  float radius;
  float height;
} bgc_capsule_t;

typedef struct {
  plane_t planes[MAX_CLIP_PLANES];
  int num_planes;
} bgc_clip_t;

typedef struct {
  vec3_t vel;
} bgc_motion_t;

typedef struct {
  quat_t move_rot;
  bool is_grounded;
} bgc_pmove_t;

typedef struct {
  edict_t *edict;
  
  bgc_transform_t *bgc_transform;
  bgc_client_t *bgc_client;
  bgc_clip_t *bgc_clip;
  bgc_capsule_t *bgc_capsule;
  bgc_motion_t *bgc_motion;
  bgc_pmove_t *bgc_pmove;
  
  bsp_node_t *bsp;
} bgame_t;

void bgs_player_move(bgame_t *bg);
void bgs_player_look(bgame_t *bg);
void bgs_clip_capsule_bsp(bgame_t *bg);
void bgs_motion_gravity(bgame_t *bg);
void bgs_motion_clip(bgame_t *bg);
void bgs_motion_integrate(bgame_t *bg);
void bgs_player_test_grounded(bgame_t *bg);

#endif
