#include "renderer.h"

#include "gl.h"
#include "vertex.h"
#include "../common/log.h"
#include "../common/zone.h"

static void renderer_init_projection_matrix(renderer_t *renderer)
{
  renderer->projection_matrix = mat4x4_init_perspective(720.0 / 1280.0, 90 * M_PI / 180.0, 0.1, 1000);
}

static void renderer_gl_init()
{
  glClearColor(0.2f, 0.7f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_FRONT);
  glDepthFunc(GL_LESS);
}

bool renderer_init(renderer_t *renderer)
{
  renderer_gl_init();
  
  mesh_pool_init(&renderer->mesh_pool, 32 * 1024);
  
  basic_shader_init(&renderer->basic_shader);
  basic_shader_bind(&renderer->basic_shader);
  
  renderer_init_projection_matrix(renderer);
  renderer_load_model(renderer, &renderer->bg_models[BG_MDL_FUMO_CIRNO], "assets/mdl/fumo_cirno.mdl");
  renderer->bg_models_vertex_ptr = mesh_pool_vertex_pointer(&renderer->mesh_pool);
  
  renderer->map_model = (r_model_t) {0};
  
  return true;
}

void renderer_setup_view_projection_matrix(renderer_t *renderer, const cgame_t *cg)
{
  vec3_t view_origin = cg->bg.transform[cg->player_entity].position;
  quat_t view_angle = cg->bg.transform[cg->player_entity].rotation;
  
  vec3_t inverted_view_origin = vec3_mulf(view_origin, -1);
  quat_t inverted_view_angle = quat_conjugate(view_angle);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(inverted_view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(inverted_view_angle);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  renderer->view_projection_matrix = mat4x4_mul(view_matrix, renderer->projection_matrix);
}

#define R_MASK_DRAW_ENTITIES (BGC_TRANSFORM | BGC_MODEL)
void renderer_draw_entities(renderer_t *renderer, const cgame_t *cg)
{
  for (int i = 0; i < cg->edict.num_entities; i++) {
    if ((cg->edict.entities[i] & R_MASK_DRAW_ENTITIES) != R_MASK_DRAW_ENTITIES)
      continue;
    
    mat4x4_t translation_matrix = mat4x4_init_translation(cg->bg.transform[i].position);
    mat4x4_t rotation_matrix = mat4x4_init_rotation(cg->bg.transform[i].rotation);
    
    mat4x4_t model_matrix = mat4x4_mul(rotation_matrix, translation_matrix);
    mat4x4_t model_view_projection_matrix = mat4x4_mul(model_matrix, renderer->view_projection_matrix);
    
    basic_shader_set_mvp(&renderer->basic_shader, model_view_projection_matrix);
    
    renderer_draw_model(renderer, &renderer->bg_models[cg->bg.model[i]]);
  }
}

void renderer_draw_map(renderer_t *renderer)
{
  basic_shader_set_mvp(&renderer->basic_shader, renderer->view_projection_matrix);
  renderer_draw_model(renderer, &renderer->map_model);
}

void renderer_render_player_view(renderer_t *renderer, const cgame_t *cg)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  renderer_setup_view_projection_matrix(renderer, cg);
  
  renderer_draw_map(renderer);
  renderer_draw_entities(renderer, cg);
}
