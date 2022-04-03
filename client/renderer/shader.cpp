#include "renderer.h"

#include "../../common/log.h"

GLuint compile_shader(GLuint type, const char *src)
{
  int success;
  static char info[1024];
  
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 1024, NULL, info);
    LOG_ERROR("compile_shader") << std::endl << info;
  }
  
  return shader;
}

GLuint link_shader(GLuint vertex_shader, GLuint fragment_shader)
{
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  
  glLinkProgram(program);
  
  glDetachShader(program, vertex_shader);
  glDetachShader(program, fragment_shader);
  
  return program;
}

shader_t::shader_t(const char *src_vertex, const char *src_fragment)
{
  GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, src_vertex);
  GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, src_fragment);
  
  m_program = link_shader(vertex_shader, fragment_shader);
  
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void shader_t::bind()
{
  glUseProgram(m_program);
}
