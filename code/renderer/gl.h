#ifndef GL_H
#define GL_H

#ifdef EMSCRIPTEN
  #include <SDL_opengles2.h>
#else
  #include <stdbool.h>
  #include <GL/glew.h>
  #include "../common/log.h"
  
  inline static bool gl_init()
  {
    glewExperimental = true;
    
    GLenum status = glewInit();
    if (status != GLEW_OK)
      log_printf(LOG_ERROR, "init_gl(): failed to initialize GLEW: %s", glewGetErrorString(status));
    
    return status == GLEW_OK;
  }
#endif

#include <SDL2/SDL_opengl.h>

#endif
