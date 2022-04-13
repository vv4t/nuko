#ifndef TEXTURE_H
#define TEXTURE_H

#include "gl.h"
#include <stdbool.h>

typedef GLuint texture_t;

bool texture_load(texture_t *texture, const char *path);
void texture_bind(texture_t texture);

#endif
