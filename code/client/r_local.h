#ifndef R_LOCAL_H
#define R_LOCAL_H

#include "renderer.h"

#include "gl.h"
#include "client.h"
#include "mdl_file.h"
#include "../common/log.h"
#include "../common/zone.h"
#include "../common/vertex.h"

#define VERTEX_ATTRIB_0 3
#define VERTEX_ATTRIB_1 3
#define VERTEX_ATTRIB_2 2

#define MAX_LIGHTS      32

#define ASPECT_RATIO    ((float) (720.0 / 1280.0))
// #define ASPECT_RATIO    ((float) (480.0 / 640.0))

#define HUD_GRID_SIZE   ((float) 32.0 / 128.0)

typedef GLuint  block_t;
typedef GLuint  texture_t;

typedef struct {
  vec3_t  pos;
  float   intensity;
  vec4_t  color;
} light_t;

typedef enum {
  HUD_ALIGN_CENTER,
  HUD_ALIGN_BOTTOM_LEFT
} hud_align_t;

typedef struct {
  hud_align_t     hud_align;
  vec2_t          scr_pos;
  vec2_t          scr_size;
  vec2_t          uv_pos;
  vec2_t          uv_size;
} hud_def_t;

typedef enum {
  HUD_CROSSHAIR,
  HUD_HEALTH_OVERLAY,
  HUD_HEALTH_LABEL,
  HUD_HEALTH_DIGIT_0,
  HUD_HEALTH_DIGIT_1,
  HUD_HEALTH_DIGIT_2,
  MAX_HUD
} r_hud_t;

typedef int r_light_t;

typedef struct {
  GLuint  offset;
  GLuint  size;
} r_mesh_t;

typedef struct {
  texture_t       texture;
} r_material_t;

typedef struct {
  GLuint          program;
  GLuint          ul_mvp;
  GLuint          ul_model;
} r_shader_t;

typedef struct {
  int             material_id;
  r_mesh_t        mesh;
} r_mesh_group_t;

typedef struct {
  r_mesh_group_t  *mesh_groups;
  int             num_mesh_groups;
  
  r_material_t    *materials;
  int             num_materials;
} r_model_t;

typedef struct {
  mat4x4_t        projection_matrix;
  mat4x4_t        view_projection_matrix;
  
  texture_t       hud_texture;
  r_shader_t      hud_shader;
  r_mesh_t        hud_mesh;
  hud_def_t       hud_defs[MAX_HUD];
  
  r_model_t       map_model;
  
  r_shader_t      cg_shader;
  r_model_t       cg_models[MAX_BG_MODEL];
  
  block_t         light_block;
  bool            light_active[MAX_LIGHTS];
  
  int             static_vbo_ptr;
  
  GLuint          vbo;
  int             vbo_size;
  int             vbo_ptr;
} renderer_t;

extern renderer_t r;

//
// r_map.c
//
void      r_draw_map();
bool      r_map_load_meshes(const map_t *map);
bool      r_map_load_materials(const map_t *map);

//
// r_light.c
//
void      r_light_init();
void      r_remove_light(r_light_t light);
void      r_set_light(r_light_t light, vec3_t pos, float intensity, vec4_t color);
r_light_t r_new_light();
//
// r_mesh.c
//
void      r_vbo_init();
void      r_vbo_reset(int vbo_ptr);
bool      r_new_mesh(r_mesh_t *mesh, int num_vertices);
bool      r_load_mesh(r_mesh_t *mesh, const vertex_t *vertices, int num_vertices);
bool      r_sub_mesh(const r_mesh_t *mesh, const vertex_t *vertices, int offset, int num_vertices);
void      r_draw_mesh(const r_mesh_t *mesh);

//
// r_model.c
//
bool      r_load_model(r_model_t *model, const char *path);
void      r_draw_model(const r_model_t *model);

//
// r_hud.c
//
bool      r_hud_init();
bool      r_init_hud_shader();
bool      r_init_hud_mesh();
void      r_hud_update_health();
void      r_render_hud();
void      hud_init_rect(vertex_t *vertices, const hud_def_t *hud_def);

//
// r_cgame.c
//
bool      r_cg_init();
bool      r_init_cg_models();
bool      r_init_cg_shader();
void      r_render_cgame();
void      r_setup_view_projection_matrix();
void      r_draw_entities();

#endif
