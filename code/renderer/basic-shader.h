#ifndef BASIC_SHADER_H
#define BASIC_SHADER_H

#include "shader.h"
#include "../common/nk-math.h"
#include <stdbool.h>

typedef struct {
  shader_t shader;
  GLuint ul_mvp;
} basic_shader_t;

bool basic_shader_init(basic_shader_t *basic_shader);
void basic_shader_bind(basic_shader_t *basic_shader);
void basic_shader_set_mvp(basic_shader_t *basic_shader, mat4x4_t mvp);

#endif
