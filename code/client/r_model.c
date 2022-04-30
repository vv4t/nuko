#include "r_local.h"

#include "mdl_file.h"
#include "../common/log.h"
#include "../common/zone.h"

bool r_load_model(r_model_t *model, const char *path)
{
  mdl_t mdl;
  if (!mdl_load(&mdl, path)) {
    log_printf(LOG_ERROR, "model_load_mdl(): failed to load mdl file '%s'\n", path);
    return false;
  }
  
  int num_mdl_vertices;
  mdl_vertex_t *mdl_vertices = mdl_load_vertices(&mdl, &num_mdl_vertices);
  
  int num_mdl_mesh_groups;
  mdl_mesh_group_t *mdl_mesh_groups = mdl_load_mesh_groups(&mdl, &num_mdl_mesh_groups);
  
  model->num_mesh_groups = num_mdl_mesh_groups;
  model->mesh_groups = zone_alloc(model->num_mesh_groups * sizeof(r_mesh_group_t));
  
  for (int i = 0; i < model->num_mesh_groups; i++) {
    model->mesh_groups[i].material_id = mdl_mesh_groups[i].material_id;
    
    int vertexofs = mdl_mesh_groups[i].vertexofs;
    int vertexlen = mdl_mesh_groups[i].vertexlen;
    
    vertex_t *vertices = (vertex_t*) &mdl_vertices[vertexofs];
    
    if (!r_new_mesh(&model->mesh_groups[i].mesh, vertices, vertexlen)) {
      log_printf(LOG_ERROR, "r_load_brushes(): failed to load mesh");
      return false;
    }
  }
  
  int num_mdl_materials;
  mdl_material_t *mdl_materials = mdl_load_materials(&mdl, &num_mdl_materials);
  
  model->num_materials = num_mdl_materials;
  model->materials = zone_alloc(model->num_materials * sizeof(r_material_t));
  
  for (int i = 0; i < model->num_materials; i++) {
    char full_name[128];
    sprintf(full_name, "assets/tex/%s.png", mdl_materials[i].name); // buffer overflow somehow? rumao
    
    if (!gl_load_texture(&model->materials[i].texture, full_name)) {
      log_printf(LOG_ERROR, "r_load_materials(): failed to load texture '%s'", full_name);
      return false;
    }
  }
  
  zone_free(mdl_vertices);
  zone_free(mdl_mesh_groups);
  
  return true;
}

void r_draw_model(const r_model_t *model)
{
  for (int i = 0; i < model->num_mesh_groups; i++) {
    int material_id = model->mesh_groups[i].material_id;
    glBindTexture(GL_TEXTURE_2D, model->materials[material_id].texture);
    r_draw_mesh(&model->mesh_groups[i].mesh);
  }
}
