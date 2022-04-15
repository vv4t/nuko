#include "mdl-file.h"

#include "../common/log.h"
#include "../common/zone.h"

bool mdl_load(mdl_t *mdl, const char *path)
{
  mdl->file = fopen(path, "rb");
  
  if (!mdl->file) {
    log_printf(LOG_ERROR, "mdl_load(): failed to open %s", path);
    return false;
  }
  
  fread(&mdl->header, 1, sizeof(mdl_header_t), mdl->file);
  
  return true;
}

mdl_vertex_t *mdl_load_vertices(const mdl_t *mdl, int *num_vertices)
{
  return (mdl_vertex_t*) lump_load(
    mdl->file,
    &mdl->header.lumps[MDL_LUMP_VERTICES],
    sizeof(mdl_vertex_t),
    num_vertices);
}

mdl_mesh_group_t *mdl_load_mesh_groups(const mdl_t *mdl, int *num_mesh_groups)
{
  return (mdl_mesh_group_t*) lump_load(
    mdl->file,
    &mdl->header.lumps[MDL_LUMP_MESH_GROUPS],
    sizeof(mdl_mesh_group_t),
    num_mesh_groups);
}

mdl_material_t *mdl_load_materials(const mdl_t *mdl, int *num_materials)
{
  return (mdl_material_t*) lump_load(
    mdl->file,
    &mdl->header.lumps[MDL_LUMP_MATERIALS],
    sizeof(mdl_material_t),
    num_materials);
}
