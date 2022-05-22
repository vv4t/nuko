#include "r_local.h"

bool r_map_load_materials(const map_t *map)
{
  // Free previous materials
  if (r.map_model.materials)
    zone_free(r.map_model.materials);
  
  int num_map_materials;
  map_material_t *map_materials = map_load_materials(map, &num_map_materials);
  
  // Allocate dynamic array for materials
  r.map_model.num_materials = num_map_materials;
  r.map_model.materials = zone_alloc(r.map_model.num_materials * sizeof(r_material_t));
  
  // Load in all the materials as GL textures
  for (int i = 0; i < r.map_model.num_materials; i++) {
    char full_name[128];
    if (snprintf(full_name, sizeof(full_name), "assets/mtl/%s.png", map_materials[i].name) >= sizeof(full_name)) {
      log_printf(LOG_ERROR, "r_map_load_materials(): material path too long");
      return false;
    }
    
    if (!gl_load_texture(&r.map_model.materials[i].texture, full_name)) {
      log_printf(LOG_ERROR, "r_map_load_materials(): failed to load texture '%s'", full_name);
      return false;
    }
  }
  
  return true;
}

bool r_map_load_meshes(const map_t *map)
{
  // Free previous mesh groups
  if (r.map_model.mesh_groups)
    zone_free(r.map_model.mesh_groups);
  
  
  int num_map_vertices;
  vertex_t *map_vertices = map_load_vertices(map, &num_map_vertices);
  
  int num_map_mesh_groups;
  map_mesh_group_t *map_mesh_groups = map_load_mesh_groups(map, &num_map_mesh_groups);
  
  // Allocate dynamic array for mesh groups
  r.map_model.num_mesh_groups = num_map_mesh_groups;
  r.map_model.mesh_groups = zone_alloc(r.map_model.num_mesh_groups * sizeof(r_mesh_group_t));
  
  // Load in each mesh group
  for (int i = 0; i < r.map_model.num_mesh_groups; i++) {
    r.map_model.mesh_groups[i].material_id = map_mesh_groups[i].material_id;
    
    int vertexofs = map_mesh_groups[i].vertexofs;
    int vertexlen = map_mesh_groups[i].vertexlen;
    
    vertex_t *vertices = &map_vertices[vertexofs];
    
    if (!r_load_mesh(&r.map_model.mesh_groups[i].mesh, vertices, vertexlen)) {
      log_printf(LOG_ERROR, "r_load_brushes(): failed to load mesh");
      return false;
    }
  }
  
  zone_free(map_vertices);
  zone_free(map_mesh_groups);
  
  return true;
}

void r_map_load_lights(const map_t *map)
{
  int num_map_lights;
  map_light_t *map_lights = map_load_lights(map, &num_map_lights);
  
  for (int i = 0; i < num_map_lights; i++) {
    vec4_t color = vec4_init(
      map_lights[i].color.x,
      map_lights[i].color.y,
      map_lights[i].color.z,
      1.0f);
    
    r_light_t light = r_new_light();
    r_set_light(light, map_lights[i].pos, map_lights[i].intensity, color);
  }
}

bool r_new_map(const map_t *map)
{
  r_vbo_reset(r.static_vbo_ptr);
  r_light_reset();
  
  memset(r.client_shoot_light, -1, sizeof(r.client_shoot_light));
  
  if (!r_map_load_materials(map)) {
    log_printf(LOG_ERROR, "r_new_map(): failed to load materials");
    return false;
  }
  
  if (!r_map_load_meshes(map)) {
    log_printf(LOG_ERROR, "r_new_map(): failed to load brushes");
    return false;
  }
  
  r_map_load_lights(map);
  
  return true;
}

void r_draw_map()
{
  mat4x4_t map_model_matrix = mat4x4_init_identity();
  
  glUniformMatrix4fv(r.light_shader.ul_mvp, 1, GL_FALSE, r.view_projection_matrix.m);
  glUniformMatrix4fv(r.light_shader.ul_model, 1, GL_FALSE, map_model_matrix.m);
  
  r_draw_model(&r.map_model);
}
