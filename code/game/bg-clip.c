#include "bg-local.h"

static void capsule_clip_bsp_R(
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
    capsule_clip_bsp_R(clip, capsule, transform, node->ahead, min_plane, min_dist);
  
  if (min_plane_dist < 0.0f) {
    if (min_plane_dist > min_dist) {
      min_plane = &node->plane;
      min_dist = min_plane_dist;
    }
    
    if (node->type == BSP_BRUSH_SOLID) {
      clip->planes[clip->num_planes] = *min_plane;
      clip->num_planes++;
    }
    
    capsule_clip_bsp_R(clip, capsule, transform, node->behind, min_plane, min_dist);
  }
}


#define BG_MASK_CLIP_CAPSULE_BSP (BGC_TRANSFORM | BGC_CLIP | BGC_CAPSULE)
void bgame_clip_capsule_bsp(bgame_t *bgame)
{
  for (int i = 0; i < bgame->edict->num_entities; i++) {
    if ((bgame->edict->entities[i] & BG_MASK_CLIP_CAPSULE_BSP) != BG_MASK_CLIP_CAPSULE_BSP)
      continue;
    
    bgame->clip[i].num_planes = 0;
    capsule_clip_bsp_R(
      &bgame->clip[i],
      &bgame->capsule[i],
      &bgame->transform[i],
      bgame->bsp,
      &bgame->bsp->plane,
      -100);
  }
}
