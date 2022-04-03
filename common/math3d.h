#ifndef MATH3D_H
#define MATH3D_H

#define _USE_MATH_DEFINES

#include <iostream>
#include <ostream>
#include <math.h>

class vec2_t;
class vec3_t;
class quat_t;

class vec2_t {
public:
  float x, y;

public:
  vec2_t(float _x = 0, float _y = 0)
  {
    x = _x;
    y = _y;
  }
  
  vec2_t operator+(const vec2_t &v) const;
  vec2_t operator-(const vec2_t &v) const;
  vec2_t operator*(const vec2_t &v) const;
  vec2_t operator*(float f) const;
  vec2_t operator/(float f) const;
  vec2_t &operator+=(const vec2_t &v);
  vec2_t &operator*=(float f);
  
  float length() const;
  float dot(const vec2_t &v) const;
  float to_angle() const;
  vec2_t normalize() const;
  vec2_t rotate(float angle) const;
  vec2_t vabs() const;
  vec2_t vfloor() const;
  vec2_t vround() const;
};

class vec3_t {
public:
  float x, y, z;

public:
  vec3_t(float _x = 0, float _y = 0, float _z = 0)
  {
    x = _x;
    y = _y;
    z = _z;
  }
  
  vec3_t(const vec2_t &v, float _z)
  {
    x = v.x;
    y = v.y;
    z = _z;
  }
  
  vec3_t operator+(const vec3_t &v) const;
  vec3_t operator*(float f) const;
  vec3_t operator*(const vec3_t &v) const;
  vec3_t operator/(float f) const;
  vec3_t &operator+=(const vec3_t &v);
  vec3_t &operator*=(const vec3_t &v);
  vec3_t &operator*=(float f);
  
  float length() const;
  float dot(const vec3_t &v) const;
  vec3_t normalize() const;
  vec3_t cross(const vec3_t &v) const;
  vec3_t rotate(const quat_t &q) const;
  vec3_t rotate_z(float angle) const;
  
  vec2_t xy() const;
};

class quat_t {
public:
  float x, y, z, w;

public:
  quat_t(float _x = 0, float _y = 0, float _z = 0, float _w = 1)
  {
    x = _x;
    y = _y;
    z = _z;
    w = _w;
  }
  
  quat_t &init_rotation(const vec3_t &axis, float theta);
  quat_t operator*(const quat_t &q) const;
  quat_t operator*(const vec3_t &v) const;
  quat_t &operator*=(const quat_t &q);
  
  quat_t conjugate() const;
  
  vec3_t get_xyz() const;
};

class mat4_t {
public:
  float m[16];

public:
  mat4_t()
  {
    for (int i = 0; i < 16; i++)
      m[i] = 0;
  }
  
  mat4_t operator*(const mat4_t &b) const;
  
  mat4_t &init_translation(const vec3_t &v);
  mat4_t &init_rotation(const quat_t &q);
  mat4_t &init_rotation_z(float angle);
  mat4_t &init_perspective(float aspect_ratio, float fov, float near, float far);
  mat4_t &init_orthogonal(float l, float r, float t, float b, float n, float f);
  mat4_t &init_orthogonal_perspective(float l, float r, float t, float b, float n, float f);
};

class plane2d_t {
public:
  vec2_t normal;
  float distance;

public:
  plane2d_t(vec2_t _normal = vec2_t(1, 0), float _distance = 0)
  {
    normal = _normal;
    distance = _distance;
  }
};

class plane_t {
public:
  vec3_t normal;
  float distance;

public:
  plane_t(vec3_t _normal = vec3_t(0, 1, 0), float _distance = 0)
  {
    normal = _normal;
    distance = _distance;
  }
};

inline vec2_t vec2_t::operator+(const vec2_t &v) const
{
  return vec2_t(
    x + v.x,
    y + v.y
  );
}

inline vec2_t vec2_t::operator-(const vec2_t &v) const
{
  return vec2_t(
    x - v.x,
    y - v.y
  );
}

inline vec2_t vec2_t::operator*(const vec2_t &v) const
{
  return vec2_t(
    x * v.x,
    y * v.y
  );
}

inline vec2_t vec2_t::operator*(float f) const
{
  return vec2_t(
    x * f,
    y * f
  );
}

inline vec2_t vec2_t::operator/(float f) const
{
  return vec2_t(
    x / f,
    y / f
  );
}

inline vec2_t &vec2_t::operator+=(const vec2_t &v)
{
  x += v.x;
  y += v.y;
  
  return *this;
}

inline vec2_t &vec2_t::operator*=(float f)
{
  x *= f;
  y *= f;
  
  return *this;
}

inline float vec2_t::length() const
{
  return sqrt(dot(*this));
}

inline float vec2_t::to_angle() const
{
  return atan2(y, x);
}

inline float vec2_t::dot(const vec2_t &v) const
{
  return x * v.x + y * v.y;
}

inline vec2_t vec2_t::rotate(float angle) const
{
  float rot_cos = cos(angle);
  float rot_sin = sin(angle);
  
  return vec2_t(
    x * rot_cos - y * rot_sin,
    x * rot_sin + y * rot_cos
  );
}

inline vec2_t vec2_t::normalize() const
{
  float d = length();
  
  if (d == 0)
    return vec2_t();
  
  return *this / d;
}

inline vec2_t vec2_t::vabs() const
{
  return vec2_t(
    abs(x),
    abs(y)
  );
}

inline vec2_t vec2_t::vround() const
{
  return vec2_t(
    round(x),
    round(y)
  );
}

inline vec2_t vec2_t::vfloor() const
{
  return vec2_t(
    floor(x),
    floor(y)
  );
}

inline float vec3_t::dot(const vec3_t &v) const
{
  return x * v.x + y * v.y + z * v.z;
}

inline vec3_t vec3_t::operator+(const vec3_t &v) const
{
  return vec3_t(
    x + v.x,
    y + v.y,
    z + v.z
  );
}

inline vec3_t vec3_t::operator*(const vec3_t &v) const
{
  return vec3_t(
    x * v.x,
    y * v.y,
    z * v.z
  );
}

inline vec3_t vec3_t::operator*(float f) const
{
  return vec3_t(x * f, y * f, z * f);
}

inline vec3_t vec3_t::operator/(float f) const
{
  return vec3_t(x / f, y / f, z / f);
}

inline vec3_t &vec3_t::operator+=(const vec3_t &v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  
  return *this;
}

inline vec3_t &vec3_t::operator*=(const vec3_t &v)
{
  x *= v.x;
  y *= v.y;
  z *= v.z;
  
  return *this;
}

inline vec3_t &vec3_t::operator*=(float f)
{
  x *= f;
  y *= f;
  z *= f;
  
  return *this;
}

inline float vec3_t::length() const
{
  return sqrt(dot(*this));
}

inline vec3_t vec3_t::normalize() const
{
  float d = length();
  
  if (d == 0)
    return vec3_t();
  
  return *this / d;
}

inline vec3_t vec3_t::cross(const vec3_t &v) const
{
  return vec3_t(
    y * v.z - z * v.y,
    z * v.x - x * v.z,
    x * v.y - y * v.x
  );
}

inline vec3_t vec3_t::rotate(const quat_t &q) const
{
  return (q * (*this) * q.conjugate()).get_xyz();
}

inline vec3_t vec3_t::rotate_z(float angle) const
{
  float rot_cos = cos(angle);
  float rot_sin = sin(angle);
  
  return vec3_t(
    x * rot_cos - y * rot_sin,
    x * rot_sin + y * rot_cos,
    z
  );
}

inline vec2_t vec3_t::xy() const
{
  return vec2_t(x, y);
}

inline quat_t quat_t::conjugate() const
{
  return quat_t(-x, -y, -z, w);
}

inline quat_t &quat_t::init_rotation(const vec3_t &axis, float theta)
{
  float cos_half_angle = cos(theta / 2.0f);
  float sin_half_angle = sin(theta / 2.0f);
  
  x = axis.x * sin_half_angle;
  y = axis.y * sin_half_angle;
  z = axis.z * sin_half_angle;
  w = cos_half_angle;
  
  return *this;
}

inline vec3_t quat_t::get_xyz() const
{
  return vec3_t(x, y, z);
}

inline quat_t quat_t::operator*(const quat_t &q) const
{
  return quat_t(
    w * q.x + x * q.w + y * q.z - z * q.y,
    w * q.y + y * q.w + z * q.x - x * q.z,
    w * q.z + z * q.w + x * q.y - y * q.x,
    w * q.w - x * q.x - y * q.y - z * q.z
  );
}

inline quat_t quat_t::operator*(const vec3_t &v) const
{
  return quat_t(
     w * v.x + y * v.z - z * v.y,
     w * v.y + z * v.x - x * v.z,
     w * v.z + x * v.y - y * v.x,
    -x * v.x - y * v.y - z * v.z
  );
}

inline quat_t &quat_t::operator*=(const quat_t &q)
{
  *this = *this * q;
  return *this;
}

inline mat4_t mat4_t::operator*(const mat4_t &b) const
{
  mat4_t r;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      r.m[i * 4 + j] = b.m[0 * 4 + j] * m[i * 4 + 0] +
                       b.m[1 * 4 + j] * m[i * 4 + 1] +
                       b.m[2 * 4 + j] * m[i * 4 + 2] +
                       b.m[3 * 4 + j] * m[i * 4 + 3];
                    
    }
  }
  
  return r;
}

inline mat4_t &mat4_t::init_translation(const vec3_t &v)
{
  m[0] = 1; m[4]  = 0; m[8]  = 0; m[12] = v.x;
  m[1] = 0; m[5]  = 1; m[9]  = 0; m[13] = v.y;
  m[2] = 0; m[6]  = 0; m[10] = 1; m[14] = v.z;
  m[3] = 0; m[7]  = 0; m[11] = 0; m[15] = 1;
  
  return *this;
}

inline mat4_t &mat4_t::init_rotation_z(float angle)
{
  float rot_cos = cos(angle);
  float rot_sin = sin(angle);
  
  m[0] = +rot_cos;  m[4]  = -rot_sin; m[8]  = 0; m[12] = 0;
  m[1] = +rot_sin;  m[5]  = +rot_cos; m[9]  = 0; m[13] = 0;
  m[2] = 0;         m[6]  = 0;        m[10] = 1; m[14] = 0;
  m[3] = 0;         m[7]  = 0;        m[11] = 0; m[15] = 1;
  
  return *this;
}

inline mat4_t &mat4_t::init_rotation(const quat_t &q)
{
  m[0]  = 1-2*q.y*q.y - 2*q.z*q.z;  m[4]  = 2*q.x*q.y - 2*q.z*q.w;    m[8]  = 2*q.x*q.z + 2*q.y*q.w;    m[12] = 0;
  m[1]  = 2*q.x*q.y + 2*q.z*q.w;    m[5]  = 1-2*q.x*q.x - 2*q.z*q.z;  m[9]  = 2*q.y*q.z - 2*q.x*q.w;    m[13] = 0;
  m[2]  = 2*q.x*q.z - 2*q.y*q.w;    m[6]  = 2*q.y*q.z + 2*q.x*q.w;    m[10] = 1-2*q.x*q.x - 2 *q.y*q.y; m[14] = 0;
  m[3]  = 0;                        m[7]  = 0;                        m[11] = 0;                        m[15] = 1;
  
  return *this;
}

inline mat4_t &mat4_t::init_perspective(float aspect_ratio, float fov, float near, float far)
{
  float tan_fov = 1 / tan(fov / 2);
  float ar_tan_fov = aspect_ratio * tan_fov;
  
  float z_scale = (-near - far) / (near - far);
  float z_offset = (2 * far * near) / (near - far);
  
  m[0]  = ar_tan_fov; m[4]  = 0;        m[8]  = 0;        m[12] = 0;
  m[1]  = 0;          m[5]  = tan_fov;  m[9]  = 0;        m[13] = 0;
  m[2]  = 0;          m[6]  = 0;        m[10] = z_scale;  m[14] = z_offset;
  m[3]  = 0;          m[7]  = 0;        m[11] = 1;        m[15] = 0;
  
  return *this;
}

inline mat4_t &mat4_t::init_orthogonal(float l, float r, float t, float b, float n, float f)
{
  m[0] = 2 / (r - l);   m[4] = 0;             m[8]  = 0;            m[12] = -(r + l) / (r - l);
  m[1] = 0;             m[5] = 2 / (t - b);   m[9]  = 0;            m[13] = -(t + b) / (t - b);
  m[2] = 0;             m[6] = 0;             m[10] = -2 / (f - n); m[14] = -(f + n) / (f - n);
  m[3] = 0;             m[7] = 0;             m[11] = 0;            m[15] = 1;
  
  return *this;
}

inline mat4_t &mat4_t::init_orthogonal_perspective(float l, float r, float t, float b, float n, float f)
{
  m[0] = 2 / (r - l);   m[4] = 0;             m[8]  = 0;            m[12] = -(r + l) / (r - l);
  m[1] = 0;             m[5] = 2 / (t - b);   m[9]  = +2 / (f - n); m[13] = -(t + b) / (t - b);
  m[2] = 0;             m[6] = 0;             m[10] = -2 / (f - n); m[14] = -(f + n) / (f - n);
  m[3] = 0;             m[7] = 0;             m[11] = 0;            m[15] = 1;
  
  return *this;
}

inline std::ostream &operator<<(std::ostream &os, vec2_t const &v)
{
  return os << "vec2_t(" << v.x << ", " << v.y << ")";
}

inline std::ostream &operator<<(std::ostream &os, vec3_t const &v)
{
  return os << "vec3_t(" << v.x << ", " << v.y << ", " << v.z << ")";
}

inline std::ostream &operator<<(std::ostream &os, mat4_t const &m)
{
          os << "mat4_t(" << m.m[0] << ", " << m.m[4] << ", " << m.m[8] << ", "  << m.m[12] << ", "  << std::endl;
          os << "       " << m.m[1] << ", " << m.m[5] << ", " << m.m[9] << ", "  << m.m[13] << ", "  << std::endl;
          os << "       " << m.m[2] << ", " << m.m[6] << ", " << m.m[10] << ", " << m.m[14] << ", "  << std::endl;
  return  os << "       " << m.m[3] << ", " << m.m[7] << ", " << m.m[11] << ", " << m.m[15] << ")"   << std::endl;
}


#endif
