#ifndef BSP_H
#define BSP_H

#include "../common/nk-math.h"

typedef enum {
  BSP_BRUSH_EMPTY,
  BSP_BRUSH_SOLID
} bsp_brush_t;

typedef struct bsp_node_s {
  bsp_brush_t       type;
  plane_t           plane;
  struct bsp_node_s *behind;
  struct bsp_node_s *ahead;
} bsp_node_t;

inline static bsp_node_t bsp_node_init(
  bsp_brush_t       type,
  plane_t           plane,
  bsp_node_t        *behind,
  bsp_node_t        *ahead)
{
  return (bsp_node_t) {
    .type = type,
    .plane = plane,
    .behind = behind,
    .ahead = ahead
  };
}

#endif
