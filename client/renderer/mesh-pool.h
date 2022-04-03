#ifndef MESH_POOL
#define MESH_POOL

#include "gl.h"
#include "vertex.h"

#include <vector>

typedef struct {
  GLuint offset;
  GLuint size;
} mesh_t;

class mesh_pool_t {
private:
  GLuint m_vbo;
  GLuint m_vao;
  GLuint m_vertex_ptr;

public:
  mesh_pool_t(int size);
  
  mesh_t  new_mesh(int size);
  void    sub_mesh(const mesh_t &mesh, vertex_t *vertices, int num_vertices);
};

#endif
