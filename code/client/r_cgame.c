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
  
  glUniform3fv(r.light_shader.ul_view_pos, 1, (float*) &cg.tween.transform[cg.ent_client].position);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, r.skybox_texture);
  glActiveTexture(GL_TEXTURE0);
  
  r_draw_map();
  r_draw_entities();
  r_draw_attack();
  r_draw_particle();
  
  glClear(GL_DEPTH_BUFFER_BIT);
  
  r_draw_weapon();
}

bool r_init_cg_models()
{
  if (!r_load_model(&r.cg_models[BG_MDL_SKULL], "assets/mdl/skull.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load BG_MDL_SKULL");
    return false;
  }
  
  if (!r_load_model(&r.cg_models[BG_MDL_SKULL2], "assets/mdl/skull2.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load BG_MDL_SKULL2");
    return false;
  }
  
  if (!r_load_model(&r.cg_models[BG_MDL_PISTOL], "assets/mdl/pistol.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load BG_MDL_PISTOL");
    return false;
  }
  
  if (!r_load_model(&r.cg_models[BG_MDL_KATANA], "assets/mdl/katana.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load BG_MDL_KATANA");
    return false;
  }
  
  if (!r_load_model(&r.bullet_model, "assets/mdl/bullet.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load bullet_model");
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

#define R_DRAW_ENTITIES (BGC_TRANSFORM | BGC_MODEL)
void r_draw_entities()
{
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & R_DRAW_ENTITIES) != R_DRAW_ENTITIES)
      continue;
    
    // Calculate the model matrix
    mat4x4_t translation_matrix = mat4x4_init_translation(cg.tween.transform[i].position);
    mat4x4_t rotation_matrix = mat4x4_init_rotation(cg.bg.transform[i].rotation);
    mat4x4_t model_matrix = mat4x4_mul(rotation_matrix, translation_matrix);
    
    // Calculate the MVP
    mat4x4_t model_view_projection_matrix = mat4x4_mul(model_matrix, r.view_projection_matrix);
    
    glUniformMatrix4fv(r.light_shader.ul_mvp, 1, GL_FALSE, model_view_projection_matrix.m);
    glUniformMatrix4fv(r.light_shader.ul_model, 1, GL_FALSE, model_matrix.m);
    
    r_draw_model(&r.cg_models[cg.bg.model[i]]);
  }
}

#define R_DRAW_WEAPON (BGC_WEAPON)
void r_draw_weapon()
{
  if ((cg.edict.entities[cg.ent_client] & R_DRAW_WEAPON) != R_DRAW_WEAPON)
    return;
  
  int attack_time = weapon_attribs[cg.bg.weapon[cg.ent_client]].attack_time;
  float interp = ((float) (attack_time - cg.tween.attack[cg.ent_client].next_attack1) / (float) attack_time);
  
  const weapon_model_t *weapon_model = &weapon_models[cg.bg.weapon[cg.ent_client]];
  
  mat4x4_t translation_matrix = mat4x4_init_translation(cg.bg.transform[cg.ent_client].position);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(cg.bg.transform[cg.ent_client].rotation);
  mat4x4_t model_matrix = mat4x4_mul(rotation_matrix, translation_matrix);
  mat4x4_t anim_matrix = weapon_model->anim_weapon(interp);
  mat4x4_t model_anim_matrix = mat4x4_mul(model_matrix, anim_matrix);
  mat4x4_t anim_projection_matrix = mat4x4_mul(anim_matrix, r.projection_matrix);
  
  glUniformMatrix4fv(r.light_shader.ul_model, 1, GL_FALSE, model_anim_matrix.m);
  glUniformMatrix4fv(r.light_shader.ul_mvp, 1, GL_FALSE, anim_projection_matrix.m);
  
  r_draw_model(&r.cg_models[weapon_model->model]);
}

#define R_DRAW_ATTACK (BGC_TRANSFORM | BGC_ATTACK | BGC_WEAPON)
void r_draw_attack()
{
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & R_DRAW_ATTACK) != R_DRAW_ATTACK)
      continue;
    
    int attack_time = weapon_attribs[cg.bg.weapon[i]].attack_time;
    float interp = ((float) (attack_time - cg.tween.attack[i].next_attack1) / (float) attack_time);
    
    if (interp > 1.0)
      continue;
    
    weapon_models[cg.bg.weapon[i]].anim_attack(&cg.bg.attack[i], interp);
  }
}

#define R_DRAW_PARTICLE (BGC_TRANSFORM | BGC_PARTICLE)
void r_draw_particle()
{
  r_particle_frame();
  
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & R_DRAW_PARTICLE) != R_DRAW_PARTICLE)
      continue;
    
    if (cg.tween.particle[i].now_time > cg.bg.particle[i].end_time)
      continue;
    
    float interp = (float) cg.tween.particle[i].now_time / (float) cg.bg.particle[i].end_time;
    
    r_particle_emit(cg.bg.transform[i].position, interp, i);
  }
  
  r_particle_draw();
}

mat4x4_t anim_weapon_pistol(float interp)
{
  if (interp > 1.0) {
    return mat4x4_init_identity();
  } else {
    return mat4x4_init_translation(vec3_init(0.0f, 0.0f, -(1.0-cos(interp * M_PI * 2.0)) * 0.1));
  }
}

mat4x4_t anim_weapon_katana(float interp)
{
  if (interp >= 0.6f) {
    return mat4x4_init_identity();
  } else if (interp > 0.3f) {
    return mat4x4_init_scale(vec3_init(0.0f, 0.0f, 0.0f));
  } else {
    vec3_t axis = vec3_normalize(vec3_init(-0.1f, 1.0f, 0.0f));
    quat_t rotation = quat_init_rotation(axis, 13.0 * interp);
    return mat4x4_init_rotation(rotation);
  }
}

void anim_attack_pistol(const bg_attack_t *attack, float interp)
{
  // Calculate the bullet's position based on when it was shot
  vec3_t offset = vec3_rotate(vec3_init(0.8f, -0.3f, 2.0f), attack->direction);
  vec3_t bullet_origin = vec3_add(attack->origin, offset); // Shift the bullet down a bit so it is visible when shot
  vec3_t bullet_dir = vec3_rotate(vec3_init(0.0f, 1.0f, 35.0f), attack->direction); // Align it with the client's rtation
  vec3_t bullet_pos = vec3_add(bullet_origin, vec3_mulf(bullet_dir, interp)); // Linearly interpolate where the bullet should be
  
  // Build its model matrix
  mat4x4_t translation_matrix = mat4x4_init_translation(bullet_pos);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(attack->direction);
  mat4x4_t model_matrix = mat4x4_mul(rotation_matrix, translation_matrix);
  
  // Calculate the MVP
  mat4x4_t model_view_projection_matrix = mat4x4_mul(model_matrix, r.view_projection_matrix);
  
  glUniformMatrix4fv(r.light_shader.ul_mvp, 1, GL_FALSE, model_view_projection_matrix.m);
  glUniformMatrix4fv(r.light_shader.ul_model, 1, GL_FALSE, model_matrix.m);
  
  glUniform1i(r.light_shader.ul_glow, 1); // Enable glow for bullets
  r_draw_model(&r.bullet_model); // Draw the bullet
  glUniform1i(r.light_shader.ul_glow, 0); // Disable glow
}

void anim_attack_katana(const bg_attack_t *attack, float interp)
{
  // The katana does not emit anything
}
