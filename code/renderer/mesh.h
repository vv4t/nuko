#ifndef MESH_H
#define MESH_H

#include "gl.h"

typedef struct {
  GLuint offset;
  GLuint size;
} mesh_t;

void mesh_draw(const mesh_t *mesh);

#endif
