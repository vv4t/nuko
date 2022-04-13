#include "bg-local.h"

#include "../common/sys.h"

#define BG_MASK_MOTION_CLIP (BGC_MOTION | BGC_CLIP | BGC_CAPSULE)
void bgame_motion_clip(bgame_t *bgame)
{
  for (int i = 0; i < bgame->edict->num_entities; i++) {
    if ((bgame->edict->entities[i] & BG_MASK_MOTION_CLIP) != BG_MASK_MOTION_CLIP)
      continue;
    
    for (int j = 0; j < bgame->clip[i].num_planes; j++) {
      plane_t plane = bgame->clip[i].planes[j];
      
      float top_plane_dist = vec3_dot(bgame->transform[i].position, plane.normal) - plane.distance;
      float bottom_plane_dist = top_plane_dist - bgame->capsule[i].height * plane.normal.y;
      
      float lambda_pos = fmin(top_plane_dist, bottom_plane_dist) - bgame->capsule[i].radius;
      float lambda_vel = vec3_dot(bgame->motion[i].velocity, plane.normal);
      
      if (lambda_pos < 0) {
        vec3_t j_pos = vec3_mulf(plane.normal, -lambda_pos);
        bgame->transform[i].position = vec3_add(bgame->transform[i].position, j_pos);
        
        if (lambda_vel < 0) {
          vec3_t j_vel = vec3_mulf(plane.normal, -lambda_vel);
          bgame->motion[i].velocity = vec3_add(bgame->motion[i].velocity, j_vel);
        }
      }
    }
  }
}

#define BG_MASK_MOTION_GRAVITY (BGC_MOTION)
void bgame_motion_gravity(bgame_t *bgame)
{
  for (int i = 0; i < bgame->edict->num_entities; i++) {
    if ((bgame->edict->entities[i] & BG_MASK_MOTION_GRAVITY) != BG_MASK_MOTION_GRAVITY)
      continue;
    
    bgame->motion[i].velocity.y -= BG_GRAVITY * BG_TIMESTEP;
  }
}

#define BG_MASK_MOTION_INTEGRATE (BGC_MOTION | BGC_TRANSFORM)
void bgame_motion_integrate(bgame_t *bgame)
{
  for (int i = 0; i < bgame->edict->num_entities; i++) {
    if ((bgame->edict->entities[i] & BG_MASK_MOTION_INTEGRATE) != BG_MASK_MOTION_INTEGRATE)
      continue;
    
    bgame->transform[i].position.x += bgame->motion[i].velocity.x * BG_TIMESTEP;
    bgame->transform[i].position.y += bgame->motion[i].velocity.y * BG_TIMESTEP;
    bgame->transform[i].position.z += bgame->motion[i].velocity.z * BG_TIMESTEP;
  }
}
