#include "bgame.h"

#define BG_TIMESTEP 0.015
#define BG_GRAVITY 18.0

#include "../common/sys.h"

#define BGS_MOTION_CLIP (BGC_MOTION | BGC_CLIP | BGC_CAPSULE)
void bgs_motion_clip(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BGS_MOTION_CLIP) != BGS_MOTION_CLIP)
      continue;
    
    for (int j = 0; j < bg->bgc_clip[i].num_planes; j++) {
      plane_t plane = bg->bgc_clip[i].planes[j];
      
      float top_plane_dist = vec3_dot(bg->bgc_transform[i].pos, plane.normal) - plane.distance;
      float bottom_plane_dist = top_plane_dist - bg->bgc_capsule[i].height * plane.normal.y;
      
      float lambda_pos = fmin(top_plane_dist, bottom_plane_dist) - bg->bgc_capsule[i].radius;
      float lambda_vel = vec3_dot(bg->bgc_motion[i].vel, plane.normal);
      
      if (lambda_pos < 0) {
        vec3_t j_pos = vec3_mulf(plane.normal, -lambda_pos);
        bg->bgc_transform[i].pos = vec3_add(bg->bgc_transform[i].pos, j_pos);
        
        if (lambda_vel < 0) {
          vec3_t j_vel = vec3_mulf(plane.normal, -lambda_vel);
          bg->bgc_motion[i].vel = vec3_add(bg->bgc_motion[i].vel, j_vel);
        }
      }
    }
  }
}

#define BGS_MOTION_GRAVITY (BGC_MOTION)
void bgs_motion_gravity(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BGS_MOTION_GRAVITY) != BGS_MOTION_GRAVITY)
      continue;
    
    bg->bgc_motion[i].vel.y -= BG_GRAVITY * BG_TIMESTEP;
  }
}

#define BGS_MOTION_INTEGRATE (BGC_MOTION | BGC_TRANSFORM)
void bgs_motion_integrate(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BGS_MOTION_INTEGRATE) != BGS_MOTION_INTEGRATE)
      continue;
    
    bg->bgc_transform[i].pos.x += bg->bgc_motion[i].vel.x * BG_TIMESTEP;
    bg->bgc_transform[i].pos.y += bg->bgc_motion[i].vel.y * BG_TIMESTEP;
    bg->bgc_transform[i].pos.z += bg->bgc_motion[i].vel.z * BG_TIMESTEP;
  }
}
