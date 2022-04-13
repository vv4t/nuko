#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

#include "mesh-pool.h"
#include "basic-shader.h"
#include "texture.h"
#include "../common/map-file.h"
#include "../cgame/cgame.h"

typedef struct {
  texture_t tex;
} r_mtl_t;

typedef struct {
  int mtl_id;
  mesh_t mesh;
} r_brush_group_t;

typedef struct {
  mesh_pool_t mesh_pool;
  basic_shader_t basic_shader;
  
  mat4x4_t projection_matrix;
  mat4x4_t view_projection_matrix;
  
  r_mtl_t *map_mtls;
  int num_mtls;
  
  r_brush_group_t *brush_groups;
  int num_brush_groups;
} renderer_t;

bool r_init(renderer_t *r);
void r_new_map(renderer_t *r, const map_t *map);
void r_render_player_view(renderer_t *r, const cgame_t *cg);

#endif
