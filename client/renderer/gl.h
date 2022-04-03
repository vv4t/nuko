#ifndef GL_H
#define GL_H

#ifdef EMSCRIPTEN
  #include <SDL_opengles2.h>
#else
  #include <GL/glew.h>
  #include "../../common/log.h"
  
  inline int init_gl()
  {
    glewExperimental = true;
    
    GLenum status  = glewInit();
    if (status != GLEW_OK)
      LOG_ERROR("renderer_t::renderer_t") << "failed to initialize GLEW: " << glewGetErrorString(status);
    
    return status == GLEW_OK;
  }
#endif

#include <SDL2/SDL_opengl.h>

#endif
