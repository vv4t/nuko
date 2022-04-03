#ifdef EMSCRIPTEN
  #include <emscripten/emscripten.h>
#endif

#include "app.h"

static app_t app;  

void app_update()
{
  app.update();
}

int main(int argc, char *argv[])
{
  app.init();
  
#ifdef EMSCRIPTEN
  emscripten_set_main_loop(app_update, 0, true);
#else
  while (!app.should_quit())
    app.update();
  
  app.quit();  
#endif
  
  return 0;
}
