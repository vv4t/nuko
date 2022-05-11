#include "r_local.h"

bool r_cg_init()
{
  if (!r_init_cg_models()) {
    log_printf(LOG_ERROR, "r_cg_init(): failed to load cg models");
    return false;
  }
  
  r.projection_matrix = mat4x4_init_perspective(ASPECT_RATIO, 90 * M_PI / 180.0, 0.1, 1000);
  
  return true;
}

void r_render_cgame()
{
  glEnable(GL_DEPTH_TEST);
  glUseProgram(r.light_shader.program);
  
  r_setup_view_projection_matrix();
  r_draw_map();
  r_draw_entities();
}

bool r_init_cg_models()
{
  if (!r_load_model(&r.cg_models[BG_MDL_SKULL], "assets/mdl/skull.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load BG_MDL_SKULL");
    return false;
  }
  
  return true;
}

void r_setup_view_projection_matrix()
{
  vec3_t view_origin = cg.tween.transform[cg.ent_client].position;
  quat_t view_angle = cg.bg.transform[cg.ent_client].rotation;
  
  vec3_t inverted_view_origin = vec3_mulf(view_origin, -1);
  quat_t inverted_view_angle = quat_conjugate(view_angle);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(inverted_view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(inverted_view_angle);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  r.view_projection_matrix = mat4x4_mul(view_matrix, r.projection_matrix);
  
  glUniformMatrix4fv(r.light_shader.ul_mvp, 1, GL_FALSE, r.view_projection_matrix.m);
}

#define R_MASK_DRAW_ENTITIES (BGC_TRANSFORM | BGC_MODEL)
void r_draw_entities()
{
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & R_MASK_DRAW_ENTITIES) != R_MASK_DRAW_ENTITIES)
      continue;
    
    mat4x4_t translation_matrix = mat4x4_init_translation(cg.tween.transform[i].position);
    mat4x4_t rotation_matrix = mat4x4_init_rotation(cg.bg.transform[i].rotation);
    
    mat4x4_t model_matrix = mat4x4_mul(rotation_matrix, translation_matrix);
    mat4x4_t model_view_projection_matrix = mat4x4_mul(model_matrix, r.view_projection_matrix);
    
    glUniformMatrix4fv(r.light_shader.ul_mvp, 1, GL_FALSE, model_view_projection_matrix.m);
    glUniformMatrix4fv(r.light_shader.ul_model, 1, GL_FALSE, model_matrix.m);
    
    r_draw_model(&r.cg_models[cg.bg.model[i]]);
  }
}


