#ifndef SHADER_H
#define SHADER_H

#include "gl.h"

class shader_t {
protected:
  GLuint m_program;
  
public:
  shader_t(const char *src_vertex, const char *src_fragment);
  
  void bind();
};

#endif
