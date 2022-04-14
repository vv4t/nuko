#include "shader.h"

#include "gl.h"
#include "../common/log.h"
#include <stdbool.h>

static bool compile_shader(GLuint *shader, GLuint type, const char *src)
{
  int success;
  static GLchar info[1024];
  
  *shader = glCreateShader(type);
  glShaderSource(*shader, 1, &src, NULL);
  
  glCompileShader(*shader);
  glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(*shader, 1024, NULL, info);
    log_printf(LOG_ERROR, "compile_shader(): failed to compile shader\n%s", info);
    return false;
  }
  
  return true;
}

static bool link_shader(GLuint *program, GLuint vertex_shader, GLuint fragment_shader)
{
  int success;
  static GLchar info[1024];
  
  *program = glCreateProgram();
  glAttachShader(*program, vertex_shader);
  glAttachShader(*program, fragment_shader);
  
  glLinkProgram(*program);
  glGetProgramiv(*program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(*program, 1024, NULL, info);
    log_printf(LOG_ERROR, "link_shader(): failed to link shader\n%s", info);
    return false;
  }
  
  glDetachShader(*program, vertex_shader);
  glDetachShader(*program, fragment_shader);
  
  return true;
}

bool shader_init(shader_t *shader, const char *src_vertex, const char *src_fragment)
{
  GLuint vertex_shader;
  if (!compile_shader(&vertex_shader, GL_VERTEX_SHADER, src_vertex)) {
    log_printf(LOG_ERROR, "shader_load(): failed to load fragment shader");
    return false;
  }
  
  GLuint fragment_shader;
  if (!compile_shader(&fragment_shader, GL_FRAGMENT_SHADER, src_fragment)) {
    log_printf(LOG_ERROR, "shader_load(): failed to load vertex_shader");
    return false;
  }
  
  if (!link_shader(shader, vertex_shader, fragment_shader)) {
    log_printf(LOG_ERROR, "shader_load(): failed to link shader");
    return false;
  }
  
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  
  return true;
}

void shader_bind(shader_t shader)
{
  glUseProgram(shader);
}
