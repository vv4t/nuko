#ifndef SHADER_H
#define SHADER_H

#include "gl.h"
#include <stdbool.h>

typedef GLuint shader_t;

bool shader_init(shader_t *shader, const char *src_vertex, const char *src_fragment);
void shader_bind(shader_t shader);

#endif
