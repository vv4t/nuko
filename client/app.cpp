#include "app.h"
#include <iostream>

#define WIDTH 1280
#define HEIGHT 720

app_t::app_t()
{
}

void app_t::init()
{
  SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  
  m_quit = false;
  m_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  m_gl_context = SDL_GL_CreateContext(m_window);
  
  m_renderer.init();
}

void app_t::poll()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
  switch (event.type) {
    case SDL_QUIT:
      m_quit = true;
      break;
    }
  }
}

void app_t::quit()
{
  SDL_GL_DeleteContext(m_gl_context);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void app_t::update()
{
  poll();
  
  m_cgame.update();
  m_renderer.render();
  
  SDL_GL_SwapWindow(m_window);
}

bool app_t::should_quit()
{
  return m_quit;
}
