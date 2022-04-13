#include "map-file.h"

#include "../common/sys.h"

bool map_load(map_t *map, const char *path)
{
  map->file = fopen(path, "rb");
  
  if (!map->file) {
    sys_log(SYS_ERROR, "map_load(): failed to open %s", path);
    return false;
  }
  
  fread(&map->map_header, 1, sizeof(map_header_t), map->file);
  
  return true;
}

static void *load_lump(const map_t *map, map_lump_t lump, int stride, int *count)
{
  int filelen = map->map_header.lumps[lump].filelen;
  int fileofs = map->map_header.lumps[lump].fileofs;
  
  void *data = sys_malloc(filelen);
  
  fseek(map->file, fileofs, SEEK_SET);
  fread(data, filelen, 1, map->file);
  
  if (count)
    *count = filelen / stride;
  
  return data;
}

map_vertex_t *map_load_vertices(const map_t *map, int *num_vertices)
{
  return (map_vertex_t*) load_lump(
    map,
    LUMP_VERTICES,
    sizeof(map_vertex_t),
    num_vertices);
}

map_brush_group_t *map_load_brush_groups(const map_t *map, int *num_brush_groups)
{
  return (map_brush_group_t*) load_lump(
    map,
    LUMP_BRUSH_GROUPS,
    sizeof(map_brush_group_t),
    num_brush_groups);
}

map_mtl_t *map_load_mtls(const map_t *map, int *num_mtls)
{
  return (map_mtl_t*) load_lump(
    map,
    LUMP_MTLS,
    sizeof(map_mtl_t),
    num_mtls);
}

static bsp_node_t *build_bsp_R(const map_bsp_node_t *map_bsp_nodes, int node)
{
  if (node == -1)
    return NULL;
  
  const map_bsp_node_t *map_bsp_node = &map_bsp_nodes[node];
  
  bsp_node_t *behind = build_bsp_R(map_bsp_nodes, map_bsp_node->behind);
  bsp_node_t *ahead = build_bsp_R(map_bsp_nodes, map_bsp_node->ahead);
  
  bsp_node_t *bsp_node = sys_malloc(sizeof(bsp_node_t));
  *bsp_node = bsp_node_init(map_bsp_node->type, map_bsp_node->plane, behind, ahead);
  
  return bsp_node;
}

bsp_node_t *map_load_bsp_nodes(const map_t *map)
{
  int num_bsp_nodes;
  map_bsp_node_t *map_bsp_nodes = load_lump(map, LUMP_BSP_NODES, sizeof(map_bsp_node_t), &num_bsp_nodes);
  
  return build_bsp_R(map_bsp_nodes, 0);
}
