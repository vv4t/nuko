#ifndef MAP_FILE_H
#define MAP_FILE_H

#include "bsp.h"
#include "../common/nk-math.h"
#include <stdio.h>
#include <stdbool.h>

#define MAX_MAP_LUMPS 4

typedef enum {
  LUMP_VERTICES,
  LUMP_BSP_NODES,
  LUMP_BRUSH_GROUPS,
  LUMP_MATERIALS
} map_lump_t;

typedef struct {
  int         fileofs;
  int         filelen;
} lump_t;

typedef struct {
  int           type;
  plane_t       plane;
  int           behind;
  int           ahead;
} map_bsp_node_t;

typedef struct {
  vec3_t        pos;
  vec2_t        uv;
} map_vertex_t;

typedef struct {
  int           material_id;
  int           vertexofs;
  int           vertexlen;
} map_brush_group_t;

typedef struct {
  char          name[32];
} map_material_t;

typedef struct {
  lump_t        lumps[MAX_MAP_LUMPS];
} map_header_t;

typedef struct {
  FILE          *file;
  map_header_t  map_header;
} map_t;

bool              map_load(map_t *map, const char *path);
map_vertex_t      *map_load_vertices(const map_t *map, int *num_vertices);
map_brush_group_t *map_load_brush_groups(const map_t *map, int *num_brush_groups);
map_material_t    *map_load_materials(const map_t *map, int *num_materials);
bsp_node_t        *map_load_bsp(const map_t *map);
void              map_free_bsp(bsp_node_t *bsp);

#endif
