#ifndef TEXTURE_H
#define TEXTURE_H

#include "gl.h"

typedef GLuint texture_t;

texture_t texture_load(const char *path);
void texture_bind(texture_t texture);

#endif
