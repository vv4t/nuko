#ifndef VERTEX_H
#define VERTEX_H

#include "../../common/math3d.h"

#define VERTEX_ATTRIB_0 3
#define VERTEX_ATTRIB_1 2

class vertex_t {
public:
  vec3_t pos;
  vec2_t uv;
  
  vertex_t(const vec3_t &_pos, const vec2_t &_uv)
  {
    pos = _pos;
    uv = _uv;
  }
};

#endif
