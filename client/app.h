#include <SDL2/SDL.h>

#include "renderer/renderer.h"
#include "cgame/cgame.h"

class app_t {
private:
  SDL_Window    *m_window;
  SDL_GLContext m_gl_context;
  
  bool          m_quit;
  
  cgame_t       *m_cgame;
  renderer_t    *m_renderer;
  
  void poll();

public:
  app_t();
  
  void update();
  void quit();
  
  bool should_quit();
};

