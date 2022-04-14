#include "basic-shader.h"

#include "../common/log.h"

bool basic_shader_init(basic_shader_t *basic_shader)
{
  static const char *src_basic_vertex = ""
    "#version 300 es\n"
    "layout(location = 0) in vec3 v_pos;\n"
    "layout(location = 1) in vec2 v_uv;\n"
    "out vec4 vs_pos;\n"
    "out vec2 vs_uv;\n"
    "uniform mat4 u_mvp;"
    "void main() {\n"
    " vs_uv = v_uv;\n"
    " vs_pos = u_mvp * vec4(v_pos, 1.0);\n"
    " gl_Position = vs_pos;\n"
    "}";

  static const char *src_basic_fragment = ""
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 frag_color;\n"
    "in vec4 vs_pos;\n"
    "in vec2 vs_uv;\n"
    "uniform sampler2D sampler;\n"
    "void main() {\n"
    " float l = 0.5 + 5.0 / vs_pos.z;\n"
    " if (l > 1.0) l = 1.0;\n"
    " vec4 light = vec4(l, l, l, 1.0);\n"
    " vec4 color = texture(sampler, vs_uv);\n"
    " frag_color = color * light;\n"
    "}";
  
  if (!shader_init(&basic_shader->shader, src_basic_vertex, src_basic_fragment)) {
    log_printf(LOG_ERROR, "basic_shader_init(): failed to load basic-shader");
    return false;
  }
  
  basic_shader->ul_mvp = glGetUniformLocation(basic_shader->shader, "u_mvp");
  
  return true;
}

void basic_shader_bind(basic_shader_t *basic_shader)
{
  glUseProgram(basic_shader->shader);
}

void basic_shader_set_mvp(basic_shader_t *basic_shader, mat4x4_t mvp)
{
  glUniformMatrix4fv(basic_shader->ul_mvp, 1, GL_FALSE, mvp.m);
}
