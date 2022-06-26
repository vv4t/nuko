/*
-- r_local.h --

Local definitions of renderer module.
*/
#ifndef R_LOCAL_H
#define R_LOCAL_H

#include "renderer.h"

#include "gl.h"
#include "client.h"
#include "mdl_file.h"
#include "../common/log.h"
#include "../common/zone.h"
#include "../common/vertex.h"
#include "../common/file.h"

// Size of each vertex attribute
#define VERTEX_ATTRIB_0 3 // position
#define VERTEX_ATTRIB_1 3 // normal
#define VERTEX_ATTRIB_2 2 // uv

#define MAX_LIGHTS      32

#define MAX_PARTICLES   64

#define SCR_WIDTH       1280
#define SCR_HEIGHT      720
#define ASPECT_RATIO    ((float) SCR_HEIGHT / (float) SCR_WIDTH)

#define HUD_GRID_SIZE   ((float) 32.0 / 256.0)

// Macros for OpenGL types for clarity
typedef GLuint  block_t;
typedef GLuint  texture_t;

// A light structure. This will be directly substituted into the shader.
// Therefore the data structures should be exactly the same as the shader
// definition of light_t in r_light.c
typedef struct {
  vec3_t  pos;
  float   intensity;
  vec4_t  color;
} light_t;

// HUD Alignment ID
typedef enum {
  HUD_ALIGN_CENTER,
  HUD_ALIGN_BOTTOM_LEFT
} hud_align_t;

// HUD elements defined simply as a rectanglular image.
typedef struct {
  hud_align_t     hud_align;
  vec2_t          scr_pos;
  vec2_t          scr_size;
  vec2_t          uv_pos;
  vec2_t          uv_size;
} hud_def_t;

// Models specific to the renderer
typedef enum {
  R_MDL_BULLET,
  R_MDL_HAND,
  MAX_R_MDL
} r_mdl_t;

// Unique ID of each HUD element.
typedef enum {
  HUD_CROSSHAIR,
  HUD_HEALTH_OVERLAY,
  HUD_HEALTH_LABEL,
  HUD_HEALTH_DIGIT_0,
  HUD_HEALTH_DIGIT_1,
  HUD_HEALTH_DIGIT_2,
  HUD_ROUND_TIME_OVERLAY,
  HUD_ROUND_TIME_DIGIT_0,
  HUD_ROUND_TIME_DIGIT_1,
  HUD_ROUND_TIME_DIGIT_2,
  HUD_ROUND_TIME_DIGIT_4,
  HUD_ROUND_TIME_COLON,
  HUD_WEAPON_SELECTED,
  HUD_WEAPON_PISTOL,
  HUD_WEAPON_KATANA,
  HUD_ATTACK2_COOLDOWN,
  MAX_HUD
} r_hud_t;

// Macro for light ID; should probably name this better.
typedef int r_light_t;

// Mesh definition which is just a pointer to its position in the vertex
// buffer. Further implementation details r_mesh.c
typedef struct {
  GLuint            offset;
  GLuint            size;
} r_mesh_t;

// NOTE: A material is just a texture at the moment. However, this may later
// include further properties such as specular or normal maps
typedef struct {
  texture_t         texture;
} r_material_t;

// Each shader should have its own structure where necessary to accomodate for
// all uniform variables and other data structures. However, this system could
// be improved by having a global stand in shader with universal uniforms such
// as 'mvp'. However, given that there is only one shader their respective
// objects, this is unecessary.
typedef struct {
  GLuint            program;
} r_hud_shader_t;

typedef struct {
  GLuint            program;
  GLuint            ul_mvp;
  GLuint            ul_model;
  GLuint            ul_glow;
  GLuint            ul_view_pos;
  GLuint            ul_sampler;
  GLuint            ul_skybox;
  GLuint            block;
} r_light_shader_t;

typedef struct {
  int               material_id;
  r_mesh_t          mesh;
} r_mesh_group_t;

typedef struct {
  r_mesh_group_t    *mesh_groups;
  int               num_mesh_groups;
  
  r_material_t      *materials;
  int               num_materials;
} r_model_t;

// NOTE: packaging all module variables into a single structure seems dodgy. I
// should instead just initiate them as static variables denoted by 'r_' while
// only having shared variables in the header. This would improve scoping.
typedef struct {
  // Projection matrices
  mat4x4_t          projection_matrix;
  mat4x4_t          view_projection_matrix;
  
  // HUD
  texture_t         hud_texture;
  r_hud_shader_t    hud_shader;
  r_mesh_t          hud_mesh;
  hud_def_t         hud_defs[MAX_HUD];
  
  // Models
  r_model_t         cg_models[MAX_BG_MDL];
  r_model_t         bullet_model;
  
  // NOTE: while the map uses a r_model_t, in the future it may need a separate
  // data structure to accomodate extra components such as light maps
  r_model_t         map_model;
  
  // Light
  r_light_shader_t  light_shader;
  bool              light_active[MAX_LIGHTS];
  
  // Mesh
  int               static_vbo_ptr; // stack pointer for all static meshes
  
  GLuint            vbo;
  int               vbo_size;
  int               vbo_ptr;
  
  // Particle
  r_mesh_t          particle_mesh;
  int               num_particles;
  
  // HDR
  r_mesh_t          screen_mesh;
  GLuint            hdr_fbo;
  GLuint            hdr_rbo;
  GLuint            hdr_shader;
  GLuint            color_buffer;
  
  // Skybox
  GLuint            skybox_ul_mvp;
  GLuint            skybox_texture;
  GLuint            skybox_shader;
  r_mesh_t          skybox_mesh;
} renderer_t;

extern renderer_t r;

// NOTE: functions denoted with 'r_draw' means it only draws geometry while
// those with 'r_render' are actual rendering pipelines

//
// r_map.c
//
void      r_draw_map();
bool      r_map_load_meshes(const map_t *map);
bool      r_map_load_materials(const map_t *map);
void      r_map_load_lights(const map_t *map);

//
// r_light.c
//
bool      r_light_init();
bool      r_init_light_shader();
void      r_init_light_block();
void      r_light_reset();
void      r_remove_light(r_light_t light);
void      r_set_light(r_light_t r_light, vec3_t pos, float intensity, vec4_t color);
r_light_t r_new_light();

//
// -- r_mesh.c --
// 
// Handles visual geometry, "mesh", allocation of vertices.
// 
// One large buffer is allocated. The renderer then "allocates" meshes in a stack
// "going up" by adding to the stack pointer and returning where it was
// originally. Meshes simply point to a location in the stack and how large it is.
// Because of this a single specific mesh cannot be freed, only the stack pointer
// can be reset to a previous point.
// 
// NOTE: a buffer is generated for only a single type of vertex (pos, normal, uv).
// I am unsure of how accomodating this design will be for multiple different
// vertex specifications in the future.
//
void      r_vbo_init();
bool      r_new_mesh(r_mesh_t *mesh, int num_vertices);
void      r_draw_mesh(const r_mesh_t *mesh);
void      r_draw_sub_mesh(const r_mesh_t *mesh, int offset, int size);

// A combination of 'r_new_mesh' and 'r_sub_mesh'. This allows for the
// generation of a mesh from a pre-allocated vertex buffer in a single call
bool      r_load_mesh(r_mesh_t *mesh, const vertex_t *vertices, int num_vertices);

// Reset the stack pointer to an earlier point
void      r_vbo_reset(int vbo_ptr);

// "Substitute" - replace - vertices in a mesh
bool      r_sub_mesh(const r_mesh_t *mesh, const vertex_t *vertices, int offset, int num_vertices);

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
void      r_hud_update_weapon();
void      r_render_hud();
void      hud_init_rect(vertex_t *vertices, const hud_def_t *hud_def);

//
// r_cgame.c
//
// Draws the client view given by 'cgame_t' in 'client.h'
//
bool      r_cg_init();
bool      r_init_cg_models();
void      r_render_cgame();
void      r_draw_entities();
void      r_draw_particle();

// Draw the local player's weapon
void      r_draw_weapon();

// Because the engine can't load in animations, animations are done as a
// function of time returning a transformation matrix where t < 1.0 means that
// the attack is active. Attacks may involve projectile.
void      anim_attack_pistol(const bg_attack_t *attack, float interp);
void      anim_attack_katana(const bg_attack_t *attack, float interp);
mat4x4_t  anim_weapon_pistol(float interp);
mat4x4_t  anim_weapon_katana(float interp);

typedef struct {
  bg_model_t  model;
  void        (*anim_attack)(const bg_attack_t *attack, float interp);
  mat4x4_t    (*anim_weapon)(float interp);
} weapon_model_t;

static const weapon_model_t weapon_models[] = {
  { .model = BG_MDL_PISTOL, .anim_attack = anim_attack_pistol, .anim_weapon = anim_weapon_pistol },
  { .model = BG_MDL_KATANA, .anim_attack = anim_attack_katana, .anim_weapon = anim_weapon_katana }
};

// - Combines the translation and rotation of the camera into a transformation matrix. -
// This is then combined with the camera's projection matrix to produce a 3D
// effect. The resulting transformation matrix orientates objects relative to
// the camera and with a 3D perspective effect.
// Produce a view matrix once per frame before hand so subsequent draw
// functions can just multiply a model matrix with the view projection matrix.
void      r_setup_view_projection_matrix();

// - Draws player's attacks -
// The current way which the rendere handles attacks is somewhat dodgy.
// It draws it as a visual representation rather than from the actual game
// logic itself i.e. what you see as a bullet doesn't mean anything in the
// game and is just a visual effect.
void      r_draw_attack();

//
// r_particle.c
// Draws particles
//

bool      r_particle_init();
void      r_particle_frame();
void      r_particle_emit(vec3_t pos, float interp, int seed);
void      r_particle_render();
void      r_particle_draw();


//
// r_hdr.c
// HDR
//

bool      r_hdr_init();
void      r_hdr_begin();
void      r_hdr_end();
void      r_hdr_draw();

//
// r_skybox.c
// Skybox
//

bool      r_skybox_init();
void      r_skybox_render();

#endif
