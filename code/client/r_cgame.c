#include "r_local.h"

// NOTE: some components can be interpolated such as 'cg.bg.transform' or
// 'cg.bg.attack'. Instead of directly referencing it, these components should
// be referenced through 'cg.tween.{component}' as to get the interpolated
// value

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
  r_draw_attack();
}

bool r_init_cg_models()
{
  if (!r_load_model(&r.cg_models[BG_MDL_SKULL], "assets/mdl/skull.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load BG_MDL_SKULL");
    return false;
  }
  
  if (!r_load_model(&r.bullet_model, "assets/mdl/bullet.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load bullet model");
    return false;
  }
  
  return true;
}

void r_setup_view_projection_matrix()
{
  vec3_t view_origin = cg.tween.transform[cg.ent_client].position;
  quat_t view_angle = cg.bg.transform[cg.ent_client].rotation;
  
  // The actual transformation performed due to camera orientation is inverse,
  // e.g. if you move right objects relative to you move left.
  vec3_t inverted_view_origin = vec3_mulf(view_origin, -1);
  quat_t inverted_view_angle = quat_conjugate(view_angle);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(inverted_view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(inverted_view_angle);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  r.view_projection_matrix = mat4x4_mul(view_matrix, r.projection_matrix);
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

#define R_DRAW_ATTACK (BGC_TRANSFORM | BGC_ATTACK)
void r_draw_attack()
{
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & R_MASK_DRAW_ENTITIES) != R_MASK_DRAW_ENTITIES)
      continue;
    
    float interp = 4 * ((float) (BG_ATTACK_TIME - cg.tween.attack[i].next_attack) / (float) BG_ATTACK_TIME);
    
    if (interp > 0.0 && interp < 1.0) {
      vec3_t bullet_origin = vec3_add(cg.tween.transform[i].position, vec3_init(0.0f, -1.0f, 0.0f)); // Shift the bullet down a bit so it is visible when shot
      vec3_t bullet_dir = vec3_rotate(vec3_init(0.0f, 1.0f, 15.0f), cg.bg.transform[i].rotation); // Align it with the client's rtation
      vec3_t bullet_pos = vec3_add(bullet_origin, vec3_mulf(bullet_dir, interp)); // Linearly interpolate where the bullet should be
  
      mat4x4_t translation_matrix = mat4x4_init_translation(bullet_pos);
      mat4x4_t rotation_matrix = mat4x4_init_rotation(cg.bg.transform[i].rotation);
      
      mat4x4_t model_matrix = mat4x4_mul(rotation_matrix, translation_matrix);
      mat4x4_t model_view_projection_matrix = mat4x4_mul(model_matrix, r.view_projection_matrix);
      
      glUniformMatrix4fv(r.light_shader.ul_mvp, 1, GL_FALSE, model_view_projection_matrix.m);
      glUniformMatrix4fv(r.light_shader.ul_model, 1, GL_FALSE, model_matrix.m);
      
      glUniform1i(r.light_shader.ul_glow, 1); // Enable glow for bullets
      r_draw_model(&r.bullet_model);
      glUniform1i(r.light_shader.ul_glow, 0); // Disable glow
    }
  }
}
