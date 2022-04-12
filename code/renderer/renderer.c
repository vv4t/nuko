#include "renderer.h"

#include "gl.h"
#include "../common/sys.h"
#include "../common/vertex.h"
#include <stdlib.h>

void r_init_projection_matrix(renderer_t *r)
{
  r->projection_matrix = mat4x4_init_perspective(720.0 / 1280.0, 90 * M_PI / 180.0, 0.1, 100);
}

void r_init_map_textures(renderer_t *r)
{
  r->map_textures[MTL_GRASS] = texture_load("../../assets/mtl/grass.png");
  r->map_textures[MTL_CONCRETE] = texture_load("../../assets/mtl/concrete.png");
  r->map_textures[MTL_BRICK] = texture_load("../../assets/mtl/brick.png");
  r->map_textures[MTL_BUILDING] = texture_load("../../assets/mtl/building.png");
}

bool r_init(renderer_t *r)
{
  glClearColor(0.2f, 0.7f, 1.0f, 1.0f);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_FRONT);
  glDepthFunc(GL_LESS);
  
  basic_shader_init(&r->basic_shader);
  mesh_pool_init(&r->mesh_pool, 4096);
  basic_shader_bind(&r->basic_shader);
  
  r_init_map_textures(r);
  r_init_projection_matrix(r);
  
  return true;
}

void r_new_map(renderer_t *r, const map_t *map)
{
  int num_map_vertices;
  map_vertex_t *map_vertices = map_load_vertices(map, &num_map_vertices);
  
  int num_map_brush_groups;
  map_brush_group_t *map_brush_groups = map_load_brush_groups(map, &num_map_brush_groups);
  
  r->num_brush_groups = num_map_brush_groups;
  r->brush_groups = sys_malloc(r->num_brush_groups * sizeof(r_brush_group_t));
  
  for (int i = 0; i < r->num_brush_groups; i++) {
    r->brush_groups[i].mtl = map_brush_groups[i].mtl;
    mesh_pool_new_mesh(
      &r->mesh_pool,
      &r->brush_groups[i].mesh,
      (vertex_t*) &map_vertices[map_brush_groups[i].vertexofs],
      map_brush_groups[i].vertexlen);
  }
  
  free(map_vertices);
  free(map_brush_groups);
}

void r_setup_view_projection_matrix(renderer_t *r, const cgame_t *cg)
{
  vec3_t inverted_view_origin = vec3_mulf(cg->bgc_transform[cg->entity_player].pos, -1);
  quat_t inverted_view_angle = quat_conjugate(cg->bgc_transform[cg->entity_player].rot);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(inverted_view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(inverted_view_angle);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  r->view_projection_matrix = mat4x4_mul(view_matrix, r->projection_matrix);
}

void r_draw_map(renderer_t *r)
{
  basic_shader_set_mvp(&r->basic_shader, r->view_projection_matrix);
  
  for (int i = 0; i < r->num_brush_groups; i++) {
    texture_bind(r->map_textures[r->brush_groups[i].mtl]);
    mesh_draw(&r->brush_groups[i].mesh);
  }
}

void r_render_player_view(renderer_t *r, const cgame_t *cg)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  r_setup_view_projection_matrix(r, cg);
  r_draw_map(r);
}
