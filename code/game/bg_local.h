/*
-- bg_local.h --

Local definitions for bgame.h
*/
#include "bgame.h"

#define BG_MIN_INCLINE  0.8
#define BG_FRICTION     4.0
#define BG_GRAVITY      18.0

//
// bg_motion.c
// Physics
//

// Apply gravity
void  bg_motion_gravity(bgame_t *bg);

// Clip velocity based on collision
void  bg_motion_clip(bgame_t *bg);

// Integrate the entity's motion into their position
void  bg_motion_integrate(bgame_t *bg);

//
// bg_pmove.c
// Player movement
//

// Accelerate the player based on the direction they wish to move
void  pm_accelerate(bg_motion_t *motion, vec3_t wish_dir, float accel, float wish_speed);

// Update the state of the player based on external factors.
// e.g. pm->grounded = true if they are standing
void  bg_pm_check_pos(bgame_t *bg);

void  bg_pm_check_weapon(bgame_t *bg);

void  bg_pm_walk_move(bgame_t *bg);
void  bg_pm_free_look(bgame_t *bg);
void  bg_pm_attack(bgame_t *bg);

//
// bg_clip.c
// Collision
//

// Navigate the BSP to check which planes the capsule is colliding with
void  clip_capsule_bsp_R(
  bg_clip_t             *clip,
  const bg_capsule_t    *capsule,
  const bg_transform_t  *transform,
  const bsp_node_t      *node,
  const plane_t         *min_plane,
  float                 min_dist);

// Clip all colliders
void  bg_clip_capsule_bsp(bgame_t *bg);

//
// bg_particle.c
// Particles
//

void  bg_particle_tick(bgame_t *bg);
