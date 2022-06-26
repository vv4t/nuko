#include "r_local.h"

renderer_t r;

bool r_init()
{
  // Initialize OpenGL settings
  glClearColor(0.10f, 0.07f, 0.07f, 1.0f);
  
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  
  glCullFace(GL_FRONT);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // Initialize vertex buffer
  r_vbo_init(32000);
  
  // Initialize HDR
  if (!r_hdr_init()) {
    log_printf(LOG_ERROR, "r_init(): failed to initialize HDR");
    return false;
  }
  
  // Initialize HUD
  if (!r_hud_init()) {
    log_printf(LOG_ERROR, "r_init(): failed to initialize hud");
    return false;
  }
  
  // Initialize particle
  if (!r_particle_init()) {
    log_printf(LOG_ERROR, "r_init(): failed to initialize particle");
    return false;
  }
  
  // Initialize client game renderer
  if (!r_cg_init()) {
    log_printf(LOG_ERROR, "r_init(): failed to initialize cgame");
    return false;
  }
  
  // Initialize light
  if (!r_light_init()) {
    log_printf(LOG_ERROR, "r_init(): failed to initialize light");
    return false;
  }
  
  // Initialize skybox
  if (!r_skybox_init()) {
    log_printf(LOG_ERROR, "r_init(): failed to initialize skybox");
    return false;
  }
  
  // Clear the map model so it isn't rendered
  // This feels kind of dodgy, perhaps a flag for if it's been initialised?
  r.map_model = (r_model_t) {0};
  
  // Now that all the static meshes are allocated, mark where it is for when
  // dynamic meshes are to be reset.
  r.static_vbo_ptr = r.vbo_ptr;
  
  // Mesh allocation scheme
  //  _____________
  // |_____hud_____|
  // |___particle__|
  // |__cg_models__|
  // |_____..._____| <- r.static_vbo_ptr; all "dynamic" meshes to be allocated after this point
  // |_____map_____|
  //
  // No further allocation of geometry at the moment
  
  return true;
}

void r_render_client_view()
{
  r_hdr_begin();
  r_skybox_render();
  r_render_cgame();
  r_hdr_end();
  r_hdr_draw();
  
  r_render_hud();
}
