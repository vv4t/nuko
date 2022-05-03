#include "bg_local.h"

void clip_capsule_bsp_R(
  bg_clip_t             *clip,
  const bg_capsule_t    *capsule,
  const bg_transform_t  *transform,
  const bsp_node_t      *node,
  const plane_t         *min_plane,
  float                 min_dist)
{
  if (!node)
    return;
  
  float top_plane_dist = vec3_dot(transform->position, node->plane.normal) - node->plane.distance;
  float bottom_plane_dist = top_plane_dist - capsule->height * node->plane.normal.y;
  
  float min_plane_dist = fmin(top_plane_dist, bottom_plane_dist) - capsule->radius;
  float max_plane_dist = fmax(top_plane_dist, bottom_plane_dist) + capsule->radius;
  
  if (max_plane_dist > 0.0f)
    clip_capsule_bsp_R(clip, capsule, transform, node->ahead, min_plane, min_dist);
  
  if (min_plane_dist < 0.0f) {
    if (min_plane_dist > min_dist) {
      min_plane = &node->plane;
      min_dist = min_plane_dist;
    }
    
    if (node->type == BSP_BRUSH_SOLID) {
      clip->planes[clip->num_planes] = *min_plane;
      clip->num_planes++;
    }
    
    clip_capsule_bsp_R(clip, capsule, transform, node->behind, min_plane, min_dist);
  }
}


#define BG_CLIP_CAPSULE_BSP (BGC_TRANSFORM | BGC_CLIP | BGC_CAPSULE)
void bg_clip_capsule_bsp(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_CLIP_CAPSULE_BSP) != BG_CLIP_CAPSULE_BSP)
      continue;
    
    bg->clip[i].num_planes = 0;
    clip_capsule_bsp_R(
      &bg->clip[i],
      &bg->capsule[i],
      &bg->transform[i],
      bg->bsp,
      &bg->bsp->plane,
      -100);
  }
}
