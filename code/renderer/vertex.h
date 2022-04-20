#ifndef VERTEX_H
#define VERTEX_H

#include "../common/nk_math.h"

#define VERTEX_ATTRIB_0 3
#define VERTEX_ATTRIB_1 2

typedef struct {
  vec3_t pos;
  vec2_t uv;
} vertex_t;

inline static vertex_t vertex_init(vec3_t pos, vec2_t uv)
{
  return (vertex_t) {
    .pos = pos,
    .uv = uv
  };
}

#endif
