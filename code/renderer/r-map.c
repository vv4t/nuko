#include "renderer.h"

#include "../common/zone.h"

static bool renderer_map_load_materials(renderer_t *renderer, const map_t *map)
{
  if (renderer->map_model.materials)
    zone_free(renderer->map_model.materials);
  
  int num_map_materials;
  map_material_t *map_materials = map_load_materials(map, &num_map_materials);
  
  renderer->map_model.num_materials = num_map_materials;
  renderer->map_model.materials = zone_alloc(renderer->map_model.num_materials * sizeof(r_material_t));
  
  for (int i = 0; i < renderer->map_model.num_materials; i++) {
    char full_name[128];
    sprintf(full_name, "../../assets/mtl/%s.png", map_materials[i].name); // buffer overflow somehow? rumao
    
    if (!texture_load(&renderer->map_model.materials[i].texture, full_name)) {
      log_printf(LOG_ERROR, "renderer_load_materials(): failed to load texture '%s'", full_name);
      return false;
    }
  }
  
  return true;
}

static bool renderer_map_load_meshes(renderer_t *renderer, const map_t *map)
{
  if (renderer->map_model.mesh_groups)
    zone_free(renderer->map_model.mesh_groups);
  
  int num_map_vertices;
  map_vertex_t *map_vertices = map_load_vertices(map, &num_map_vertices);
  
  int num_map_mesh_groups;
  map_mesh_group_t *map_mesh_groups = map_load_mesh_groups(map, &num_map_mesh_groups);
  
  renderer->map_model.num_mesh_groups = num_map_mesh_groups;
  renderer->map_model.mesh_groups = zone_alloc(renderer->map_model.num_mesh_groups * sizeof(r_mesh_group_t));
  
  for (int i = 0; i < renderer->map_model.num_mesh_groups; i++) {
    renderer->map_model.mesh_groups[i].material_id = map_mesh_groups[i].material_id;
    
    int vertexofs = map_mesh_groups[i].vertexofs;
    int vertexlen = map_mesh_groups[i].vertexlen;
    
    vertex_t *vertices = (vertex_t*) &map_vertices[vertexofs];
    
    if (!mesh_pool_new_mesh(&renderer->mesh_pool, &renderer->map_model.mesh_groups[i].mesh, vertices, vertexlen)) {
      log_printf(LOG_ERROR, "renderer_load_brushes(): failed to load mesh");
      return false;
    }
  }
  
  zone_free(map_vertices);
  zone_free(map_mesh_groups);
  
  return true;
}

bool renderer_new_map(renderer_t *renderer, const map_t *map)
{
  mesh_pool_reset(&renderer->mesh_pool, renderer->cg_models_vertex_ptr);
  
  if (!renderer_map_load_materials(renderer, map)) {
    log_printf(LOG_ERROR, "renderer_new_map(): failed to load materials");
    return false;
  }
  
  if (!renderer_map_load_meshes(renderer, map)) {
    log_printf(LOG_ERROR, "renderer_new_map(): failed to load brushes");
    return false;
  }
  
  return true;
}
