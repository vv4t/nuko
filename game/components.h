#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "../common/math3d.h"
#include "../common/usercmd.h"

typedef struct {
  vec3_t pos;
  quat_t rot;
} c_transform_t;

typedef struct {
  vec3_t old_pos;
  vec3_t accel;
} c_motion_t;

typedef struct {
  usercmd_t usercmd;
} c_client_t;

#endif
