#include "gl.h"

bool gl_init()
{
#ifndef __EMSCRIPTEN__
  glewExperimental = true;
  
  GLenum status = glewInit();
  if (status != GLEW_OK)
    log_printf(LOG_ERROR, "init_gl(): failed to initialize GLEW: %s", glewGetErrorString(status));
  
  return status == GLEW_OK;
#else
  return true;
#endif
}

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

bool gl_load_shader(GLuint *shader, const char *src_vertex, const char *src_fragment)
{
  GLuint vertex_shader;
  if (!compile_shader(&vertex_shader, GL_VERTEX_SHADER, src_vertex)) {
    log_printf(LOG_ERROR, "shader_load(): failed to load vertex shader");
    return false;
  }
  
  GLuint fragment_shader;
  if (!compile_shader(&fragment_shader, GL_FRAGMENT_SHADER, src_fragment)) {
    log_printf(LOG_ERROR, "shader_load(): failed to load fragment shader");
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

bool gl_load_texture(GLuint *texture, const char *path)
{
  SDL_Surface *bitmap = IMG_Load(path);
  
  if (!bitmap) {
    log_printf(LOG_ERROR, "texture_load(): could not load %s", path);
    return false;
  }
  
  glGenTextures(1, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->w, bitmap->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  return true;
}
