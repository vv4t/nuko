#include "map_file.h"

#include "../common/log.h"
#include "../common/zone.h"

bool map_load(map_t *map, const char *path)
{
  map->file = fopen(path, "rb");
  
  if (!map->file) {
    log_printf(LOG_ERROR, "map_load(): failed to open %s", path);
    return false;
  }
  
  fread(&map->header, 1, sizeof(map_header_t), map->file);
  
  return true;
}

vertex_t *map_load_vertices(const map_t *map, int *num_vertices)
{
  return (vertex_t*) lump_load(
    map->file,
    &map->header.lumps[MAP_LUMP_VERTICES],
    sizeof(vertex_t),
    num_vertices);
}

map_mesh_group_t *map_load_mesh_groups(const map_t *map, int *num_mesh_groups)
{
  return (map_mesh_group_t*) lump_load(
    map->file,
    &map->header.lumps[MAP_LUMP_MESH_GROUPS],
    sizeof(map_mesh_group_t),
    num_mesh_groups);
}

map_material_t *map_load_materials(const map_t *map, int *num_materials)
{
  return (map_material_t*) lump_load(
    map->file,
    &map->header.lumps[MAP_LUMP_MATERIALS],
    sizeof(map_material_t),
    num_materials);
}

static bsp_node_t *build_bsp_R(const map_bsp_node_t *map_bsp_nodes, int node)
{
  if (node == -1)
    return NULL;
  
  const map_bsp_node_t *map_bsp_node = &map_bsp_nodes[node];
  
  bsp_node_t *behind = build_bsp_R(map_bsp_nodes, map_bsp_node->behind);
  bsp_node_t *ahead = build_bsp_R(map_bsp_nodes, map_bsp_node->ahead);
  
  bsp_node_t *bsp_node = zone_alloc(sizeof(bsp_node_t));
  *bsp_node = bsp_node_init(map_bsp_node->type, map_bsp_node->plane, behind, ahead);
  
  return bsp_node;
}

bsp_node_t *map_load_bsp(const map_t *map)
{
  int num_bsp_nodes;
  map_bsp_node_t *map_bsp_nodes = lump_load(
    map->file,
    &map->header.lumps[MAP_LUMP_BSP_NODES],
    sizeof(map_bsp_node_t),
    &num_bsp_nodes);
  
  return build_bsp_R(map_bsp_nodes, 0);
}

static void free_bsp_R(bsp_node_t *node)
{
  if (!node)
    return;
  
  free_bsp_R(node->behind);
  free_bsp_R(node->ahead);
  
  zone_free(node);
}

void map_free_bsp(bsp_node_t *bsp)
{
  free_bsp_R(bsp);
}
