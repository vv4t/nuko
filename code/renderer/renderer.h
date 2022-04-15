#ifndef RENDERER_H
#define RENDERER_H

#include "texture.h"
#include "mesh-pool.h"
#include "basic-shader.h"
#include "../cgame/cgame.h"
#include "../game/map-file.h"
#include <stdbool.h>

typedef struct {
  texture_t       texture;
} r_material_t;

typedef struct {
  int             material_id;
  mesh_t          mesh;
} r_mesh_group_t;

typedef struct {
  r_mesh_group_t  *mesh_groups;
  int             num_mesh_groups;
  
  r_material_t    *materials;
  int             num_materials;
} r_model_t;

typedef struct {
  mesh_pool_t     mesh_pool;
  basic_shader_t  basic_shader;
  
  mat4x4_t        projection_matrix;
  mat4x4_t        view_projection_matrix;
  
  r_model_t       map_model;
  
  r_model_t       cg_models[MAX_CG_MODEL];
  int             cg_models_vertex_ptr;
} renderer_t;

bool renderer_load_model(renderer_t *renderer, r_model_t *model, const char *path);
void renderer_draw_model(renderer_t *renderer, const r_model_t *model);

bool renderer_init(renderer_t *renderer);
bool renderer_new_map(renderer_t *renderer, const map_t *map);
void renderer_render_player_view(renderer_t *renderer, const cgame_t *cg);

#endif
