#ifndef BASIC_SHADER_H
#define BASIC_SHADER_H

#include "gl.h"
#include "shader.h"
#include "../../common/math3d.h"

class basic_shader_t : public shader_t {
private:
  GLuint  m_uloc_mvp;

public:
  basic_shader_t();
  
  void set_mvp(const mat4_t &mvp);
};

#endif
