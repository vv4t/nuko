#ifndef MDL_FILE_H
#define MDL_FILE_H

#include "../common/lump.h"
#include "../common/nk-math.h"
#include <stdio.h>
#include <stdbool.h>

typedef enum {
  MDL_LUMP_VERTICES,
  MDL_LUMP_MESH_GROUPS,
  MDL_LUMP_MATERIALS,
  MAX_MDL_LUMPS
} mdl_lump_t;

typedef struct {
  vec3_t        pos;
  vec2_t        uv;
} mdl_vertex_t;

typedef struct {
  int           material_id;
  int           vertexofs;
  int           vertexlen;
} mdl_mesh_group_t;

typedef struct {
  char          name[32];
} mdl_material_t;

typedef struct {
  lump_t        lumps[MAX_MDL_LUMPS];
} mdl_header_t;

typedef struct {
  FILE          *file;
  mdl_header_t  header;
} mdl_t;

bool              mdl_load(mdl_t *mdl, const char *path);
mdl_vertex_t      *mdl_load_vertices(const mdl_t *mdl, int *num_vertices);
mdl_mesh_group_t  *mdl_load_mesh_groups(const mdl_t *mdl, int *num_mesh_groups);
mdl_material_t    *mdl_load_materials(const mdl_t *mdl, int *num_materials);

#endif
