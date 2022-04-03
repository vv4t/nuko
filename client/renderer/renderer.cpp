#include "renderer.h"

#include "../../common/math3d.h"

#include <iostream>

renderer_t::renderer_t()
  : m_mesh_pool(1024)
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  
  vertex_t vertices[] = {
    vertex_t(vec3_t(0, 0, 0), vec2_t(0, 0)),
    vertex_t(vec3_t(1, 0, 0), vec2_t(1, 0)),
    vertex_t(vec3_t(0, 1, 0), vec2_t(0, 1)),
  };
  
  m_projection_matrix = mat4_t().init_perspective(720.0 / 1280.0, 90 * M_PI / 180.0, 0.1, 100);
  
  m_mesh = m_mesh_pool.new_mesh(3);
  m_mesh_pool.sub_mesh(m_mesh, vertices, 3);
  
  m_basic_shader.bind();
}

float t = 0.0f;

void renderer_t::render()
{
  t += 0.01f;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  mat4_t translation = mat4_t().init_translation(vec3_t(cos(t), 0, 3));
  mat4_t mvp = translation * m_projection_matrix;
  
  m_basic_shader.set_mvp(mvp);
  
  glDrawArrays(GL_TRIANGLES, m_mesh.offset, m_mesh.size);
}
