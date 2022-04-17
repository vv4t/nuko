#ifndef MESH_POOL_H
#define MESH_POOL_H

#include "gl.h"
#include "mesh.h"
#include "vertex.h"
#include <stdbool.h>

typedef struct {
  GLuint  vbo;
  GLuint  vao;
  int     vertex_size;
  int     vertex_ptr;
} mesh_pool_t;

void  mesh_pool_init(mesh_pool_t *mesh_pool, int size);
bool  mesh_pool_new_mesh(mesh_pool_t *mesh_pool, mesh_t *mesh, const vertex_t *vertices, int num_vertices);
void  mesh_pool_reset(mesh_pool_t *mesh_pool, int vertex_ptr);
int   mesh_pool_vertex_pointer(const mesh_pool_t *mesh_pool);

#endif
