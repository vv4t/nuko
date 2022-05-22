#include "r_local.h"

#include "../common/log.h"

void r_vbo_init(int vbo_size)
{
  // Reset the stack pointer
  r.vbo_size = vbo_size;
  r.vbo_ptr = 0;
  
  // Generate the GL buffer
  glGenBuffers(1, &r.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, r.vbo);
  glBufferData(GL_ARRAY_BUFFER, vbo_size * sizeof(vertex_t), 0, GL_STATIC_DRAW);

  GLuint offset = 0;
  
  // From the macros in 'r_local.h', map the vertex buffer
#ifdef VERTEX_ATTRIB_0
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, VERTEX_ATTRIB_0, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + offset);
  offset += VERTEX_ATTRIB_0;
#endif

#ifdef VERTEX_ATTRIB_1
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, VERTEX_ATTRIB_1, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + offset);
  offset += VERTEX_ATTRIB_1;
#endif

#ifdef VERTEX_ATTRIB_2
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, VERTEX_ATTRIB_2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (float*) 0 + offset);
  offset += VERTEX_ATTRIB_2;
#endif
}

void r_vbo_reset(int vbo_ptr)
{
  // Boundary checks
  if (vbo_ptr < 0 || vbo_ptr > r.vbo_size) {
    log_printf(LOG_ERROR, "r_vbo_reset(): invalid reset pointer %i/%i", vbo_ptr, r.vbo_size);
    r.vbo_ptr = 0;
    return;
  }
  
  r.vbo_ptr = vbo_ptr;
}

bool r_new_mesh(r_mesh_t *mesh, int num_vertices)
{
  // Boundary checks
  if (r.vbo_ptr + num_vertices >= r.vbo_size) {
    log_printf(
      LOG_ERROR,
      "r_load_mesh(): vertex buffer ran out of memory %i/%i",
      r.vbo_ptr + num_vertices,
      r.vbo_size);
    return false;
  }
  
  mesh->offset = r.vbo_ptr;
  mesh->size = num_vertices;
  r.vbo_ptr += num_vertices;
  
  return true;
}

bool r_sub_mesh(const r_mesh_t *mesh, const vertex_t *vertices, int offset, int num_vertices)
{
  // Boundary checks
  if (offset + num_vertices > mesh->size) {
    log_printf(LOG_ERROR, "r_sub_mesh(): out of bounds %i/%i", offset + num_vertices, mesh->size);
    return false;
  }
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    (mesh->offset + offset) * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    vertices);
  
  return true;
}

bool r_load_mesh(r_mesh_t *mesh, const vertex_t *vertices, int num_vertices)
{
  if (!r_new_mesh(mesh, num_vertices)) {
    log_printf(LOG_ERROR, "r_load_mesh(): failed to allocate mesh");
    return false;
  }
  
  if (!r_sub_mesh(mesh, vertices, 0, num_vertices)) {
    log_printf(LOG_ERROR, "r_load_mesh(): failed to substitute mesh");
    return false;
  }
  
  return true;
}

void r_draw_mesh(const r_mesh_t *mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh->offset, mesh->size);
}
