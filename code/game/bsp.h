/*
-- bsp.h --

Binary space partitioning. This is used in collision. It works by dividing the
map, a collection of convex hulls, into a binary tree wherein each node is a
plane of a hull. By navigating the tree, the space is divided, until only the
original convex hull is left. This reduces time spent testing the player
against planes.
*/
#ifndef BSP_H
#define BSP_H

#include "../common/nk_math.h"

// The type of 'brushes' or convex hulls
typedef enum {
  BSP_BRUSH_EMPTY,
  BSP_BRUSH_SOLID
} bsp_brush_t;

// A BSP node
typedef struct bsp_node_s {
  bsp_brush_t       type;
  plane_t           plane;
  
  // Rather than left or right in a traditional tree, a plane can be divided
  // into the space behind it or in front of it
  struct bsp_node_s *behind;
  struct bsp_node_s *ahead;
} bsp_node_t;

// Function for initialising a node
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
