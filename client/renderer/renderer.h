#ifndef RENDERER_H
#define RENDERER_H

#include "gl.h"
#include "mesh-pool.h"
#include "basic-shader.h"

class renderer_t {
private:
  mesh_pool_t     m_mesh_pool;
  basic_shader_t  m_basic_shader;
  
  mat4_t          m_projection_matrix;
  
  mesh_t          m_mesh;

public:
  renderer_t();
  
  void init();
  void render();
};

#endif
