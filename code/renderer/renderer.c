#include "renderer.h"

#include "gl.h"
#include "vertex.h"
#include "../common/sys.h"
#include <stdlib.h>

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
  
  mesh_pool_init(&renderer->mesh_pool, 4096);
  
  basic_shader_init(&renderer->basic_shader);
  basic_shader_bind(&renderer->basic_shader);
  
  renderer_init_projection_matrix(renderer);
  
  renderer->num_materials = 0;
  renderer->materials = NULL;
  
  renderer->num_brush_groups = 0;
  renderer->brush_groups = NULL;
  
  return true;
}

static bool renderer_load_materials(renderer_t *renderer, const map_t *map)
{
  if (renderer->materials)
    free(renderer->materials);
  
  int num_map_materials;
  map_material_t *map_materials = map_load_materials(map, &num_map_materials);
  
  renderer->num_materials = num_map_materials;
  renderer->materials = malloc(renderer->num_materials * sizeof(r_material_t));
  
  for (int i = 0; i < renderer->num_materials; i++) {
    char full_name[128];
    sprintf(full_name, "../../assets/mtl/%s.png", map_materials[i].name); // buffer overflow somehow? rumao
    
    if (!texture_load(&renderer->materials[i].texture, full_name)) {
      sys_log(SYS_ERROR, "renderer_load_materials(): failed to load texture '%s'", full_name);
      return false;
    }
  }
  
  return true;
}

static bool renderer_load_brushes(renderer_t *renderer, const map_t *map)
{
  if (renderer->brush_groups)
    free(renderer->brush_groups);
  
  int num_map_vertices;
  map_vertex_t *map_vertices = map_load_vertices(map, &num_map_vertices);
  
  int num_map_brush_groups;
  map_brush_group_t *map_brush_groups = map_load_brush_groups(map, &num_map_brush_groups);
  
  renderer->num_brush_groups = num_map_brush_groups;
  renderer->brush_groups = malloc(renderer->num_brush_groups * sizeof(r_brush_group_t));
  
  for (int i = 0; i < renderer->num_brush_groups; i++) {
    renderer->brush_groups[i].material_id = map_brush_groups[i].material_id;
    
    int vertexofs = map_brush_groups[i].vertexofs;
    int vertexlen = map_brush_groups[i].vertexlen;
    
    vertex_t *vertices = (vertex_t*) &map_vertices[vertexofs];
    
    if (!mesh_pool_new_mesh(&renderer->mesh_pool, &renderer->brush_groups[i].mesh, vertices, vertexlen)) {
      sys_log(SYS_ERROR, "renderer_load_brushes(): failed to load mesh");
      return false;
    }
  }
  
  free(map_vertices);
  free(map_brush_groups);
  
  return true;
}

bool renderer_new_map(renderer_t *renderer, const map_t *map)
{
  if (!renderer_load_materials(renderer, map)) {
    sys_log(SYS_ERROR, "renderer_new_map(): failed to load materials");
    return false;
  }
  
  if (!renderer_load_brushes(renderer, map)) {
    sys_log(SYS_ERROR, "renderer_new_map(): failed to load brushes");
    return false;
  }
  
  return true;
}

void renderer_setup_view_projection_matrix(renderer_t *renderer, const cgame_t *cgame)
{
  vec3_t inverted_view_origin = vec3_mulf(cgame->bg_transform[cgame->entity_player].position, -1);
  quat_t inverted_view_angle = quat_conjugate(cgame->bg_transform[cgame->entity_player].rotation);
  
  mat4x4_t translation_matrix = mat4x4_init_translation(inverted_view_origin);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(inverted_view_angle);
  
  mat4x4_t view_matrix = mat4x4_mul(translation_matrix, rotation_matrix);
  
  renderer->view_projection_matrix = mat4x4_mul(view_matrix, renderer->projection_matrix);
}

void renderer_draw_map(renderer_t *renderer)
{
  basic_shader_set_mvp(&renderer->basic_shader, renderer->view_projection_matrix);
  
  for (int i = 0; i < renderer->num_brush_groups; i++) {
    int material_id = renderer->brush_groups[i].material_id;
    texture_bind(renderer->materials[material_id].texture);
    mesh_draw(&renderer->brush_groups[i].mesh);
  }
}

void renderer_render_player_view(renderer_t *renderer, const cgame_t *cgame)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  renderer_setup_view_projection_matrix(renderer, cgame);
  renderer_draw_map(renderer);
}
