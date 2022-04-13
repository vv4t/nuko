#ifndef RENDERER_H
#define RENDERER_H

#include "texture.h"
#include "mesh-pool.h"
#include "basic-shader.h"
#include "../cgame/cgame.h"
#include "../common/map-file.h"
#include <stdbool.h>

typedef struct {
  texture_t       texture;
} r_material_t;

typedef struct {
  int             material_id;
  mesh_t          mesh;
} r_brush_group_t;

typedef struct {
  mesh_pool_t     mesh_pool;
  basic_shader_t  basic_shader;
  
  mat4x4_t        projection_matrix;
  mat4x4_t        view_projection_matrix;
  
  int             num_materials;
  r_material_t    *materials;
  
  int             num_brush_groups;
  r_brush_group_t *brush_groups;
} renderer_t;

bool renderer_init(renderer_t *renderer);
bool renderer_new_map(renderer_t *renderer, const map_t *map);
void renderer_render_player_view(renderer_t *renderer, const cgame_t *cg);

#endif
