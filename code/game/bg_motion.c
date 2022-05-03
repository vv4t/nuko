#include "bg_local.h"

#define BG_MOTION_CLIP (BGC_MOTION | BGC_CLIP | BGC_CAPSULE)
void bg_motion_clip(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_MOTION_CLIP) != BG_MOTION_CLIP)
      continue;
    
    for (int j = 0; j < bg->clip[i].num_planes; j++) {
      plane_t plane = bg->clip[i].planes[j];
      
      float top_plane_dist = vec3_dot(bg->transform[i].position, plane.normal) - plane.distance;
      float bottom_plane_dist = top_plane_dist - bg->capsule[i].height * plane.normal.y;
      
      float lambda_pos = fmin(top_plane_dist, bottom_plane_dist) - bg->capsule[i].radius;
      float lambda_vel = vec3_dot(bg->motion[i].velocity, plane.normal);
      
      if (lambda_pos < 0) {
        vec3_t j_pos = vec3_mulf(plane.normal, -lambda_pos);
        bg->transform[i].position = vec3_add(bg->transform[i].position, j_pos);
        
        if (lambda_vel < 0) {
          vec3_t j_vel = vec3_mulf(plane.normal, -lambda_vel);
          bg->motion[i].velocity = vec3_add(bg->motion[i].velocity, j_vel);
        }
      }
    }
  }
}

#define BG_MOTION_GRAVITY (BGC_MOTION)
void bg_motion_gravity(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_MOTION_GRAVITY) != BG_MOTION_GRAVITY)
      continue;
    
    bg->motion[i].velocity.y -= BG_GRAVITY * BG_TIMESTEP;
  }
}

#define BG_MOTION_INTEGRATE (BGC_MOTION | BGC_TRANSFORM)
void bg_motion_integrate(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_MOTION_INTEGRATE) != BG_MOTION_INTEGRATE)
      continue;
    
    vec3_t delta_pos = vec3_mulf(bg->motion[i].velocity, BG_TIMESTEP);
    
    bg->transform[i].position = vec3_add(bg->transform[i].position, delta_pos);
  }
}
