#include "basic-shader.h"

static const char *src_basic_vertex = ""
  "#version 300 es\n"
  "layout(location = 0) in vec3 v_pos;\n"
  "layout(location = 1) in vec2 v_uv;\n"
  "out vec2 vs_uv;\n"
  "uniform mat4 u_mvp;"
  "void main() {\n"
  " vs_uv = v_uv;\n"
  " gl_Position = u_mvp * vec4(v_pos, 1.0);\n"
  "}";

static const char *src_basic_fragment = ""
  "#version 300 es\n"
  "precision mediump float;\n"
  "out vec4 frag_color;\n"
  "in vec2 vs_uv;\n"
  "uniform sampler2D sampler;\n"
  "void main() {\n"
  " vec4 color = texture(sampler, vs_uv);\n"
  " frag_color = vec4(1, 1, 1, 1);\n"
  "}";

basic_shader_t::basic_shader_t()
  : shader_t(src_basic_vertex, src_basic_fragment)
{
  m_uloc_mvp = glGetUniformLocation(m_program, "u_mvp");
}

void basic_shader_t::set_mvp(const mat4_t &mvp)
{
  glUniformMatrix4fv(m_uloc_mvp, 1, GL_FALSE, mvp.m);
}
