#ifndef MESH_POOL_H
#define MESH_POOL_H

#include "gl.h"
#include "mesh.h"
#include "vertex.h"
#include <stdbool.h>

typedef struct {
  GLuint  vbo;
  GLuint  vao;
  int     vertex_ptr;
} mesh_pool_t;

void mesh_pool_init(mesh_pool_t *mesh_pool, int size);
bool mesh_pool_new_mesh(mesh_pool_t *mesh_pool, mesh_t *mesh, const vertex_t *vertices, int num_vertices);

#endif
