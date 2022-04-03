#include "mesh-pool.h"

#include "../../common/log.h"

mesh_pool_t::mesh_pool_t(int size)
{
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertex_t), 0, GL_STATIC_DRAW);
  
  int offset = 0;
  int stride = sizeof(vertex_t);
  
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, VERTEX_ATTRIB_0, GL_FLOAT, GL_FALSE, stride, (float*) 0 + offset);
  offset += VERTEX_ATTRIB_0;
  
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, VERTEX_ATTRIB_1, GL_FLOAT, GL_FALSE, stride, (float*) 0 + offset);
  offset += VERTEX_ATTRIB_1;
  
  m_vertex_ptr = size - 1; 
}

mesh_t mesh_pool_t::new_mesh(int num_vertices)
{
  if (m_vertex_ptr - num_vertices < 0)
    LOG_ERROR("mesh_pool_t::new_mesh") << "ran out of memory: " << m_vertex_ptr + num_vertices << "/" << num_vertices << std::endl;
  
  m_vertex_ptr -= num_vertices;
  
  return (mesh_t) { .offset = m_vertex_ptr, .size = (GLuint) num_vertices };
}

void mesh_pool_t::sub_mesh(const mesh_t &mesh, vertex_t *vertices, int num_vertices)
{
  glBufferSubData(
    GL_ARRAY_BUFFER,
    mesh.offset * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    vertices);
}
