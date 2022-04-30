#ifndef NK_MATH_H
#define NK_MATH_H

#define _USE_MATH_DEFINES

#include <math.h>

typedef struct {
  float x;
  float y;
} vec2_t;

typedef struct {
  float x;
  float y;
  float z;
} vec3_t;

typedef struct {
  float x;
  float y;
  float z;
  float w;
} quat_t;

typedef struct {
  vec3_t normal;
  float distance;
} plane_t;

typedef struct {
  float m[16];
} mat4x4_t;

inline static vec2_t vec2_init(float x, float y)
{
  return (vec2_t) {
    .x = x,
    .y = y
  };
}

inline static vec3_t vec3_init(float x, float y, float z)
{
  return (vec3_t) {
    .x = x,
    .y = y,
    .z = z
  };
}

inline static vec3_t vec3_add(vec3_t a, vec3_t b)
{
  return vec3_init(
    a.x + b.x,
    a.y + b.y,
    a.z + b.z);
}

inline static vec3_t vec3_sub(vec3_t a, vec3_t b)
{
  return vec3_init(
    a.x - b.x,
    a.y - b.y,
    a.z - b.z);
}

inline static vec3_t vec3_mulf(vec3_t a, float b)
{
  return vec3_init(
    a.x * b,
    a.y * b,
    a.z * b);
}

inline static float vec3_dot(vec3_t a, vec3_t b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline static float vec3_length(vec3_t v)
{
  return sqrt(vec3_dot(v, v));
}

inline static vec3_t vec3_normalize(vec3_t v)
{
  float l = vec3_length(v);
  
  if (l == 0)
    return vec3_init(0.0f, 0.0f, 0.0f);
  
  return vec3_mulf(v, 1.0 / l);
}

inline static quat_t quat_init(float x, float y, float z, float w)
{
  return (quat_t) {
    .x = x,
    .y = y,
    .z = z,
    .w = w
  };
}

inline static quat_t quat_conjugate(quat_t q)
{
  return quat_init(-q.x, -q.y, -q.z, q.w);
}

inline static quat_t quat_init_rotation(vec3_t axis, float theta)
{
	float cos_half_angle = cos(theta / 2.0f);
	float sin_half_angle = sin(theta / 2.0f);
	
	float x = axis.x * sin_half_angle;
	float y = axis.y * sin_half_angle;
	float z = axis.z * sin_half_angle;
	float w = cos_half_angle;
  
  return quat_init(x, y, z, w);
}

inline static quat_t quat_mul(quat_t a, quat_t b)
{
  return quat_init(
    a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
    a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
    a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
    a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

inline static quat_t quat_mul_vec3(quat_t a, vec3_t b)
{
  return quat_init(
     a.w * b.x + a.y * b.z - a.z * b.y,
     a.w * b.y + a.z * b.x - a.x * b.z,
     a.w * b.z + a.x * b.y - a.y * b.x,
    -a.x * b.x - a.y * b.y - a.z * b.z);
}

inline static vec3_t vec3_rotate(vec3_t v, quat_t q)
{
  quat_t q_conjugate = quat_conjugate(q);
  
  quat_t half_rot = quat_mul_vec3(q, v);
  quat_t full_rot = quat_mul(half_rot, q_conjugate);
  
  return vec3_init(full_rot.x, full_rot.y, full_rot.z);
}

inline static mat4x4_t mat4x4_init_identity() {
  mat4x4_t m;
  m.m[0]  = 1;  m.m[4]  = 0;  m.m[8]  = 0;  m.m[12] = 0;
  m.m[1]  = 0;  m.m[5]  = 1;  m.m[9]  = 0;  m.m[13] = 0;
  m.m[2]  = 0;  m.m[6]  = 0;  m.m[10] = 1;  m.m[14] = 0;
  m.m[3]  = 0;  m.m[7]  = 0;  m.m[11] = 0;  m.m[15] = 1;
  return m;
}

inline static mat4x4_t mat4x4_init_translation(vec3_t v) {
  mat4x4_t m;
  m.m[0]  = 1;  m.m[4]  = 0;  m.m[8]  = 0;  m.m[12] = v.x;
  m.m[1]  = 0;  m.m[5]  = 1;  m.m[9]  = 0;  m.m[13] = v.y;
  m.m[2]  = 0;  m.m[6]  = 0;  m.m[10] = 1;  m.m[14] = v.z;
  m.m[3]  = 0;  m.m[7]  = 0;  m.m[11] = 0;  m.m[15] = 1;
  return m;
}

inline static mat4x4_t mat4x4_mul(mat4x4_t a, mat4x4_t b)
{
  mat4x4_t m;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m.m[i * 4 + j] =  b.m[0 * 4 + j] * a.m[i * 4 + 0] +
                        b.m[1 * 4 + j] * a.m[i * 4 + 1] +
                        b.m[2 * 4 + j] * a.m[i * 4 + 2] +
                        b.m[3 * 4 + j] * a.m[i * 4 + 3];
                    
    }
  }
  
  return m;
}

inline static mat4x4_t mat4x4_init_rotation(quat_t q)
{
  mat4x4_t m;
  m.m[0]  = 1-2*q.y*q.y - 2*q.z*q.z;  m.m[4]  = 2*q.x*q.y - 2*q.z*q.w;    m.m[8]  = 2*q.x*q.z + 2*q.y*q.w;    m.m[12] = 0;
  m.m[1]  = 2*q.x*q.y + 2*q.z*q.w;    m.m[5]  = 1-2*q.x*q.x - 2*q.z*q.z;  m.m[9]  = 2*q.y*q.z - 2*q.x*q.w;    m.m[13] = 0;
  m.m[2]  = 2*q.x*q.z - 2*q.y*q.w;    m.m[6]  = 2*q.y*q.z + 2*q.x*q.w;    m.m[10] = 1-2*q.x*q.x - 2 *q.y*q.y; m.m[14] = 0;
  m.m[3]  = 0;                        m.m[7]  = 0;                        m.m[11] = 0;                        m.m[15] = 1;
  
  return m;
}

inline static mat4x4_t mat4x4_init_perspective(float aspect_ratio, float fov, float near, float far)
{
  float tan_fov = 1 / tan(fov / 2);
  float ar_tan_fov = aspect_ratio * tan_fov;
  
  float z_scale = (-near - far) / (near - far);
  float z_offset = (2 * far * near) / (near - far);
  
  mat4x4_t m;
  m.m[0]  = ar_tan_fov; m.m[4]  = 0;        m.m[8]  = 0;        m.m[12] = 0;
  m.m[1]  = 0;          m.m[5]  = tan_fov;  m.m[9]  = 0;        m.m[13] = 0;
  m.m[2]  = 0;          m.m[6]  = 0;        m.m[10] = z_scale;  m.m[14] = z_offset;
  m.m[3]  = 0;          m.m[7]  = 0;        m.m[11] = 1;        m.m[15] = 0;
  
  return m;
}

#endif
