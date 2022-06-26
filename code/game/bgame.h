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
#define BG_ATTACK2_TIME 2000 // ATTACK2 cooldown

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
  BGC_WEAPON    = (1 << 9),
  BGC_PARTICLE  = (1 << 10),
  AUX_BGC       = (1 << 11)
} bg_component_t;

// Shared entity states
typedef enum {
  BG_ES_CLIENT = BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_MODEL | BGC_PMOVE | BGC_HEALTH | BGC_ATTACK | BGC_WEAPON
} bg_entitystate_t;

typedef struct {
  float       forward;
  float       right;
  float       yaw;
  float       pitch;
  float       jump;
  float       attack1;
  float       attack2;
  int         weapon_slot;
} usercmd_t;

// Data structures prefixed by 'bg_' are components
// They should probably be renamed to 'bgc_' for clarity though

// Shared models i.e. can be seen by everyone, used to identify to clients
// which model to use for other clients
typedef enum {
  BG_MDL_SKULL,
  BG_MDL_SKULL2,
  BG_MDL_PISTOL,
  BG_MDL_KATANA,
  MAX_BG_MDL
} bg_model_t;

typedef struct {
  vec3_t    position;
  quat_t    rotation;
} bg_transform_t;

typedef enum {
  BG_WEAPON_PISTOL,
  BG_WEAPON_KATANA
} bg_weapon_t;

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
  int       now_time;
  int       end_time;
} bg_particle_t;

typedef struct {
  bool      active; // Used by subsequent systems to activate shoot behvaiour -> sv_game.c
  int       next_attack1;
  int       next_attack2;
  vec3_t    origin;
  quat_t    direction;
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
  bg_weapon_t     sv_weapon[MAX_ENTITIES];
  bg_particle_t   sv_particle[MAX_ENTITIES];
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
  bg_weapon_t     weapon[MAX_ENTITIES];
  bg_particle_t   particle[MAX_ENTITIES];
} bgame_t;

typedef struct {
  float attack_time;
  int   damage;
  void  (*attack2)(bgame_t *bgame, entity_t entity);
} weapon_attrib_t;

// bg_pmove.c
void  attack2_pistol(bgame_t *bg, entity_t entity);
void  attack2_katana(bgame_t *bg, entity_t entity);

static const weapon_attrib_t weapon_attribs[] = {
  { .attack_time = 70,    .damage = 10,   .attack2 = attack2_pistol },  // BG_WEAPON_PISTOL
  { .attack_time = 500,   .damage = 100,  .attack2 = attack2_katana }   // BG_WEAPON_KATANA
};

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
