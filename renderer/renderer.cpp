#include "renderer.h"

#include <iostream>

renderer_t::renderer_t()
{
}

void renderer_t::init()
{
#ifndef EMSCRIPTEN
  glewExperimental = true;
  
	GLenum status  = glewInit();
	if (status != GLEW_OK)
    std::cout << "renderer_t::renderer_t: failed to initialize GLEW: " << glewGetErrorString(status);
#endif
  
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void renderer_t::render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
