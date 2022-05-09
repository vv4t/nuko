#include "r_local.h"

renderer_t r;

bool r_init()
{
  glClearColor(0.2f, 0.7f, 1.0f, 1.0f);
  
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  
  glCullFace(GL_FRONT);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  r_vbo_init(32000);
  
  if (!r_hud_init()) {
    log_printf(LOG_ERROR, "r_init(): failed to load hud");
    return false;
  }
  
  if (!r_cg_init()) {
    log_printf(LOG_ERROR, "r_init(): failed to load cgame");
    return false;
  }
  
  r.map_model = (r_model_t) {0};
  
  r.static_vbo_ptr = r.vbo_ptr;
  
  return true;
}

void r_render_client_view()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  r_render_cgame();
  r_render_hud();
}
