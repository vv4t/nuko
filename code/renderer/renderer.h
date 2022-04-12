#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

#include "mesh-pool.h"
#include "basic-shader.h"
#include "texture.h"
#include "../common/map-file.h"
#include "../cgame/cgame.h"

typedef struct {
  map_brush_mtl_t mtl;
  mesh_t mesh;
} r_brush_group_t;

typedef struct {
  mesh_pool_t mesh_pool;
  basic_shader_t basic_shader;
  
  mat4x4_t projection_matrix;
  mat4x4_t view_projection_matrix;
  
  texture_t map_textures[MAX_MAP_BRUSH_MTLS];
  
  r_brush_group_t *brush_groups;
  int num_brush_groups;
} renderer_t;

bool r_init(renderer_t *r);
void r_new_map(renderer_t *r, const map_t *map);
void r_render_player_view(renderer_t *r, const cgame_t *cg);

#endif
