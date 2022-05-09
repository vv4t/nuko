#include "r_local.h"

bool r_cg_init()
{
  if (!r_init_cg_models()) {
    log_printf(LOG_ERROR, "r_cg_init(): unable to load cg models");
    return false;
  }
  
  if (!r_init_cg_shader()) {
    log_printf(LOG_ERROR, "r_cg_init(): unable to load cg shader");
    return false;
  }
  
  r.projection_matrix = mat4x4_init_perspective(ASPECT_RATIO, 90 * M_PI / 180.0, 0.1, 1000);
  
  return true;
}

void r_render_cgame()
{
  glUseProgram(r.cg_shader.program);
  
  r_setup_view_projection_matrix();
  r_draw_map();
  r_draw_entities();
}

bool r_init_cg_models()
{
  if (!r_load_model(&r.cg_models[BG_MDL_FUMO_CIRNO], "assets/mdl/fumo_cirno.mdl")) {
    log_printf(LOG_ERROR, "r_init_cg_models(): unable to load BG_MDL_FUMO_CIRNO");
    return false;
  }
  
  return true;
}

bool r_init_cg_shader()
{
  static const char *src_vertex = ""
    "#version 300 es\n"
    "layout(location = 0) in vec3 v_pos;\n"
    "layout(location = 1) in vec2 v_uv;\n"
    "out vec4 vs_pos;\n"
    "out vec2 vs_uv;\n"
    "uniform mat4 u_mvp;"
    "void main() {\n"
    " vs_uv = v_uv;\n"
    " vs_pos = u_mvp * vec4(v_pos, 1.0);\n"
    " gl_Position = vs_pos;\n"
    "}";

  static const char *src_fragment = ""
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 frag_color;\n"
    "in vec4 vs_pos;\n"
    "in vec2 vs_uv;\n"
    "uniform sampler2D sampler;\n"
    "void main() {\n"
    " float l = 0.5 + 5.0 / vs_pos.z;\n"
    " if (l > 1.0) l = 1.0;\n"
    " vec4 light = vec4(l, l, l, 1.0);\n"
    " vec4 color = texture(sampler, vs_uv);\n"
    " frag_color = color * light;\n"
    "}";
  
  if (!gl_load_shader(&r.cg_shader.program, src_vertex, src_fragment)) {
    log_printf(LOG_ERROR, "r_init_cg_shader(): failed to load shader");
    return false;
  }
  
  r.cg_shader.ul_mvp = glGetUniformLocation(r.cg_shader.program, "u_mvp");
  
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
    
    glUniformMatrix4fv(r.cg_shader.ul_mvp, 1, GL_FALSE, model_view_projection_matrix.m);
    
    r_draw_model(&r.cg_models[cg.bg.model[i]]);
  }
}