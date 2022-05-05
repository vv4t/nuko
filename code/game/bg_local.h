#include "bgame.h"

#define BG_MIN_INCLINE  0.8
#define BG_TIMESTEP     0.050
#define BG_FRICTION     4.0
#define BG_GRAVITY      18.0

// bg_motion.c
void  bg_motion_gravity(bgame_t *bg);
void  bg_motion_clip(bgame_t *bg);
void  bg_motion_integrate(bgame_t *bg);

// bg_pmove.c
void  pm_accelerate(bg_motion_t *motion, vec3_t wish_dir, float accel, float wish_speed);

void  bg_pm_check_pos(bgame_t *bg);
void  bg_pm_walk_move(bgame_t *bg);
void  bg_pm_free_look(bgame_t *bg);

// bg_clip.c
void  clip_capsule_bsp_R(
  bg_clip_t             *clip,
  const bg_capsule_t    *capsule,
  const bg_transform_t  *transform,
  const bsp_node_t      *node,
  const plane_t         *min_plane,
  float                 min_dist);

void  bg_clip_capsule_bsp(bgame_t *bg);
