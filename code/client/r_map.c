#include "r_local.h"

bool r_map_load_materials(const map_t *map)
{
  if (r.map_model.materials)
    zone_free(r.map_model.materials);
  
  int num_map_materials;
  map_material_t *map_materials = map_load_materials(map, &num_map_materials);
  
  r.map_model.num_materials = num_map_materials;
  r.map_model.materials = zone_alloc(r.map_model.num_materials * sizeof(r_material_t));
  
  for (int i = 0; i < r.map_model.num_materials; i++) {
    char full_name[128];
    sprintf(full_name, "assets/mtl/%s.png", map_materials[i].name); // buffer overflow somehow? rumao
    
    if (!gl_load_texture(&r.map_model.materials[i].texture, full_name)) {
      log_printf(LOG_ERROR, "r_load_materials(): failed to load texture '%s'", full_name);
      return false;
    }
  }
  
  return true;
}

bool r_map_load_meshes(const map_t *map)
{
  if (r.map_model.mesh_groups)
    zone_free(r.map_model.mesh_groups);
  
  int num_map_vertices;
  map_vertex_t *map_vertices = map_load_vertices(map, &num_map_vertices);
  
  int num_map_mesh_groups;
  map_mesh_group_t *map_mesh_groups = map_load_mesh_groups(map, &num_map_mesh_groups);
  
  r.map_model.num_mesh_groups = num_map_mesh_groups;
  r.map_model.mesh_groups = zone_alloc(r.map_model.num_mesh_groups * sizeof(r_mesh_group_t));
  
  for (int i = 0; i < r.map_model.num_mesh_groups; i++) {
    r.map_model.mesh_groups[i].material_id = map_mesh_groups[i].material_id;
    
    int vertexofs = map_mesh_groups[i].vertexofs;
    int vertexlen = map_mesh_groups[i].vertexlen;
    
    vertex_t *vertices = (vertex_t*) &map_vertices[vertexofs];
    
    if (!r_new_mesh(&r.map_model.mesh_groups[i].mesh, vertices, vertexlen)) {
      log_printf(LOG_ERROR, "r_load_brushes(): failed to load mesh");
      return false;
    }
  }
  
  zone_free(map_vertices);
  zone_free(map_mesh_groups);
  
  return true;
}

bool r_new_map(const map_t *map)
{
  r_vbo_reset(r.bg_models_vbo_ptr);
  
  if (!r_map_load_materials(map)) {
    log_printf(LOG_ERROR, "r_new_map(): failed to load materials");
    return false;
  }
  
  if (!r_map_load_meshes(map)) {
    log_printf(LOG_ERROR, "r_new_map(): failed to load brushes");
    return false;
  }
  
  return true;
}
