#include "bgame.h"

#include "../common/sys.h"

static void capsule_clip_bsp_R(
  bgc_clip_t *clip,
  const bgc_capsule_t *capsule,
  const bgc_transform_t *transform,
  const bsp_node_t *node,
  const plane_t *min_plane,
  float min_dist)
{
  if (!node)
    return;
  
  float top_plane_dist = vec3_dot(transform->pos, node->plane.normal) - node->plane.distance;
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


#define BGS_CLIP_CAPSULE_BSP (BGC_TRANSFORM | BGC_CLIP | BGC_CAPSULE)
void bgs_clip_capsule_bsp(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BGS_CLIP_CAPSULE_BSP) != BGS_CLIP_CAPSULE_BSP)
      continue;
    
    bg->bgc_clip[i].num_planes = 0;
    capsule_clip_bsp_R(
      &bg->bgc_clip[i],
      &bg->bgc_capsule[i],
      &bg->bgc_transform[i],
      bg->bsp,
      &bg->bsp->plane,
      -100);
  }
}
