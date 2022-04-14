#include "mesh-pool.h"

#include "../common/log.h"

static void init_gl_buffer(mesh_pool_t *mesh_pool, int size)
{
#ifndef EMSCRIPTEN
  glGenVertexArrays(1, &mesh_pool->vao);
  glBindVertexArray(mesh_pool->vbo);
#endif
  
  glGenBuffers(1, &mesh_pool->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh_pool->vbo);
  glBufferData(GL_ARRAY_BUFFER, size * sizeof(vertex_t), 0, GL_STATIC_DRAW);

  GLuint offset = 0;

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
}

void mesh_pool_init(mesh_pool_t *mesh_pool, int size)
{
  init_gl_buffer(mesh_pool, size);
  
  mesh_pool->vertex_size = size;
  mesh_pool->vertex_ptr = 0;
}

bool mesh_pool_new_mesh(mesh_pool_t *mesh_pool, mesh_t *mesh, const vertex_t *vertices, int num_vertices)
{
  if (mesh_pool->vertex_ptr + num_vertices >= mesh_pool->vertex_size) {
    log_printf(LOG_ERROR, "mesh_pool_new_mesh(): vertex buffer ran out of memory");
    return false;
  }
  
  mesh->offset = mesh_pool->vertex_ptr;
  mesh->size = num_vertices;
  mesh_pool->vertex_ptr += num_vertices;
  
  glBufferSubData(
    GL_ARRAY_BUFFER,
    mesh->offset * sizeof(vertex_t),
    num_vertices * sizeof(vertex_t),
    vertices);
  
  return true;
}

void mesh_pool_reset(mesh_pool_t *mesh_pool, int vertex_ptr)
{
  if (vertex_ptr < 0) {
    log_printf(LOG_ERROR, "mesh_pool_reset(): invalid reset pointer %i/%i", vertex_ptr, mesh_pool->vertex_size);
    return;
  }
  
  mesh_pool->vertex_ptr = vertex_ptr;
}

void mesh_draw(const mesh_t *mesh)
{
  glDrawArrays(GL_TRIANGLES, mesh->offset, mesh->size);
}
