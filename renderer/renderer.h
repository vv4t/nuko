#ifndef RENDERER_H
#define RENDERER_H

#ifdef EMSCRIPTEN
  #include <SDL_opengles2.h>
#else
  #include <GL/glew.h>
#endif

#include <SDL2/SDL_opengl.h>

class renderer_t {
private:

public:
  renderer_t();
  
  void init();
  void render();
};

#endif
