#ifndef GL_H
#define GL_H

#ifdef __EMSCRIPTEN__
  #include <SDL_opengles2.h>
#else
  #include <GL/glew.h>
#endif

#include <stdbool.h>
#include <SDL2/SDL_opengl.h>

bool gl_init();
bool gl_load_texture(GLuint *texture, const char *path);
bool gl_load_shader(GLuint *shader, const char *src_vertex, const char *src_fragment);

#endif
