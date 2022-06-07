/*
-- bgame.h --

Definitions for both games - client and server. A version of the game which is
run on both the client and the server.
*/
#ifndef BGAME_H
#define BGAME_H

#include "bsp.h"
#include "edict.h"
#include "map_file.h"
#include "../common/nk_math.h"
#include <stdbool.h>

#define MAX_CLIP_PLANES 16 // Maximum planes detected by a collider
#define BG_ATTACK_TIME 250 // Time between attacks

// Shared components
typedef enum {
  BGC_TRANSFORM = (1 << 0),
  BGC_CLIENT    = (1 << 1),
  BGC_CLIP      = (1 << 2),
  BGC_CAPSULE   = (1 << 3),
  BGC_MOTION    = (1 << 4),
  BGC_PMOVE     = (1 << 5),
  BGC_MODEL     = (1 << 6),
  BGC_HEALTH    = (1 << 7),
  BGC_ATTACK    = (1 << 8),
  AUX_BGC       = (1 << 9)
} bg_component_t;

// Shared entity states
typedef enum {
  BG_ES_CLIENT = BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_MODEL | BGC_PMOVE | BGC_HEALTH | BGC_ATTACK
} bg_entitystate_t;

typedef struct {
  float     forward;
  float     right;
  float     yaw;
  float     pitch;
  float     jump;
  float     attack;
} usercmd_t;

// Data structures prefixed by 'bg_' are components
// They should probably be renamed to 'bgc_' for clarity though

typedef enum {
  BG_MDL_SKULL,
  MAX_BG_MODEL
} bg_model_t;

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
  bool      on_ground;
} bg_pmove_t;

typedef struct {
  vec3_t    velocity;
} bg_motion_t;

typedef struct {
  int       now;
  int       max;
} bg_health_t;

typedef struct {
  bool      active; // Used by subsequent systems to activate shoot behvaiour -> sv_game.c
  int       next_attack;
} bg_attack_t;

typedef struct {
  edict_t         edict;
  
  int             round_time;
  
  // Components local to the client, i.e. visible only to them
  int             cl_entity_state;
  bg_pmove_t      cl_pmove;
  bg_motion_t     cl_motion;
  bg_health_t     cl_health;
  
  // Components global to the client
  bg_model_t      sv_model[MAX_ENTITIES];
  bg_transform_t  sv_transform[MAX_ENTITIES];
  bg_capsule_t    sv_capsule[MAX_ENTITIES];
  bg_attack_t     sv_attack[MAX_ENTITIES];
} snapshot_t;

typedef struct {
  edict_t         *edict;
  bsp_node_t      *bsp; // BSP of the map
  
  // Components
  bg_transform_t  transform[MAX_ENTITIES];
  bg_client_t     client[MAX_ENTITIES];
  bg_clip_t       clip[MAX_ENTITIES];
  bg_capsule_t    capsule[MAX_ENTITIES];
  bg_motion_t     motion[MAX_ENTITIES];
  bg_pmove_t      pmove[MAX_ENTITIES];
  bg_model_t      model[MAX_ENTITIES];
  bg_health_t     health[MAX_ENTITIES];
  bg_attack_t     attack[MAX_ENTITIES];
} bgame_t;

//
// bg_pmove.c
//

// Rotates the transform based on the camera's yaw and pitch
// This is made public as it is used by cl_cgame.c to predict the camera's
// movement outside of a frame
void  pm_free_look(bg_transform_t *transform, float yaw, float pitch);

//
// bg_main.c
//
void  bg_init(bgame_t *bg, edict_t *edict);
void  bg_new_map(bgame_t *bg, const map_t *map);
void  bg_update(bgame_t *bg);

#endif
