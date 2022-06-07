#include "r_local.h"

// Rectangle vertices template for HUD creation.
static const vertex_t hud_rect[] = {
  { .pos = { 0.0f, 0.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { 0.0f, 1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { 1.0f, 0.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { 0.0f, 1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { 1.0f, 1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { 1.0f, 0.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } }
};

static const int num_hud_rect_vertices = sizeof(hud_rect) / sizeof(vertex_t);

// Populate an array of vertices based on the hud_def
void hud_init_rect(vertex_t *vertices, const hud_def_t *hud_def)
{
  // Offset the rect depending on the alignment
  vec3_t align_pos;
  switch (hud_def->hud_align) {
  case HUD_ALIGN_CENTER:
    align_pos.x = -0.5f;
    align_pos.y = -0.5f;
    align_pos.z = 0.0f;
    break;
  case HUD_ALIGN_BOTTOM_LEFT:
    align_pos.x = 0.0f;
    align_pos.y = 0.0f;
    align_pos.z = 0.0f;
    break;
  default:
    log_printf(LOG_WARNING, "hud_init_rect(): unknown HUD_ALIGN: %i\n", hud_def->hud_align);
    return;
  }
  
  for (int i = 0; i < num_hud_rect_vertices; i++) {
    vertices[i].pos = vec3_add(
      vec3_init(hud_def->scr_pos.x, hud_def->scr_pos.y, 0.0f), // Offset the vertex position
      vec3_mul(
        vec3_add(
          hud_rect[i].pos,
          align_pos), // Offset the template by the alignment
        vec3_init(hud_def->scr_size.x * ASPECT_RATIO, hud_def->scr_size.y, 1.0f))); // Scale it based on the hud_def and aspect_ratio 
    
    // Apply to UV
    vertices[i].uv = vec2_mulf(
      vec2_add(
        hud_def->uv_pos,
        vec2_mul(
          hud_rect[i].uv,
          hud_def->uv_size)),
      HUD_GRID_SIZE);
  }
}

bool r_hud_init()
{
  if (!r_init_hud_mesh()) {
    log_printf(LOG_ERROR, "r_hud_init(): failed to initialise hud mesh");
    return false;
  }
  
  if (!r_init_hud_shader()) {
    log_printf(LOG_ERROR, "r_hud_init(): failed to load hud shader");
    return false;
  }
  
  if (!gl_load_texture(&r.hud_texture, "assets/mtl/hud.png")) {
    log_printf(LOG_ERROR, "r_load_materials(): failed to load texture 'assets/tex/hud.png'");
    return false;
  }
  
  return true;
}

bool r_init_hud_mesh()
{
  // The HUD mesh is simply an array of rectangular meshes. Each unique HUD
  // should be defined in the r_hud_t enum which serves as an ID for which
  // hud_def it belongs to. Their hud_defs are then defined here then loaded
  // into the vertex buffer.
  // NOTE: this should probably be replaced for a better system
  
  // Crosshair mesh
  r.hud_defs[HUD_CROSSHAIR] = (hud_def_t) {
    .hud_align  = HUD_ALIGN_CENTER,
    .scr_pos    = vec2_init(+0.00f, +0.00f),
    .scr_size   = vec2_init(+0.05f, +0.05f),
    .uv_pos     = vec2_init(+3.00f, +3.00f),
    .uv_size    = vec2_init(+1.00f, +1.00f) };
  
  // Health mesh
  r.hud_defs[HUD_HEALTH_OVERLAY] = (hud_def_t) {
    .hud_align  = HUD_ALIGN_BOTTOM_LEFT,
    .scr_pos    = vec2_init(-0.9f, -0.9f),
    .scr_size   = vec2_init(+0.4f, +0.2f),
    .uv_pos     = vec2_init(+0.0f, +0.0f),
    .uv_size    = vec2_init(+3.0f, +1.0f) };
  
  r.hud_defs[HUD_HEALTH_LABEL] = (hud_def_t) {
    .hud_align  = HUD_ALIGN_BOTTOM_LEFT,
    .scr_pos    = vec2_init(-0.87f, -0.85f),
    .scr_size   = vec2_init(+0.05f, +0.05f),
    .uv_pos     = vec2_init(+2.0f, +3.0f),
    .uv_size    = vec2_init(+1.0f, +1.0f) };
  
  for (int i = 0; i < 3; i++) {
    r.hud_defs[HUD_HEALTH_DIGIT_0 + i] = (hud_def_t) {
      .hud_align  = HUD_ALIGN_BOTTOM_LEFT,
      .scr_pos    = vec2_init(-0.82 + i * 0.04f, -0.85f),
      .scr_size   = vec2_init(+0.15f, +0.15f),
      .uv_pos     = vec2_init(+0.0f, +1.0f),
      .uv_size    = vec2_init(+1.0f, +1.0f) };
  }
  
  // Round time mesh
  r.hud_defs[HUD_ROUND_TIME_OVERLAY] = (hud_def_t) {
    .hud_align  = HUD_ALIGN_CENTER,
    .scr_pos    = vec2_init(+0.0f, 0.8f),
    .scr_size   = vec2_init(+0.4f, +0.2f),
    .uv_pos     = vec2_init(+0.0f, +0.0f),
    .uv_size    = vec2_init(+3.0f, +1.0f) };
  
  for (int i = 0; i < 4; i++) {
    // Weird scaling and offset adjustments to make it look better.
    // Also shift +1 to right if i > 1 to skip colon
    float digit_pos = (i - 1 + (i > 1)) * 0.04f - 0.011f;
    r.hud_defs[HUD_ROUND_TIME_DIGIT_0 + i] = (hud_def_t) {
      .hud_align  = HUD_ALIGN_CENTER,
      .scr_pos    = vec2_init(digit_pos, 0.825f),
      .scr_size   = vec2_init(+0.15f, +0.15f),
      .uv_pos     = vec2_init(+0.0f, +1.0f),
      .uv_size    = vec2_init(+1.0f, +1.0f) };
  }
  
  // Weird colon alignment to make up for white space in HUD texture
  float colon_pos = 0.04f - 0.01f;
  
  r.hud_defs[HUD_ROUND_TIME_COLON] = (hud_def_t) {
    .hud_align  = HUD_ALIGN_CENTER,
    .scr_pos    = vec2_init(colon_pos, 0.825f),
    .scr_size   = vec2_init(+0.15f, +0.15f),
    .uv_pos     = vec2_init(+3.0f, +0.0f),
    .uv_size    = vec2_init(+1.0f, +1.0f) };
  
  if (!r_new_mesh(&r.hud_mesh, MAX_HUD * num_hud_rect_vertices)) {
    log_printf(LOG_ERROR, "r_init_hud_mesh(): failed to allocate hud mesh");
    return false;
  }
  
  for (int i = 0; i < MAX_HUD; i++) {
    vertex_t hud_vertices[num_hud_rect_vertices];
    hud_init_rect(hud_vertices, &r.hud_defs[i]);
    r_sub_mesh(&r.hud_mesh, hud_vertices, i * num_hud_rect_vertices, num_hud_rect_vertices);
  }
  
  return true;
}

void r_hud_update_health()
{
  int hp = cg.bg.health[cg.ent_client].now;
  
  // Get each individual digit
  int digits[] = {
    (hp % 1000) / 100,
    (hp % 100) / 10,
    (hp % 10) / 1
  };
  
  for (int i = 0; i < 3; i++) {
    r_hud_t hud_id = HUD_HEALTH_DIGIT_0 + i;
    
    // Replace the digit by shifting the UV pos of the hud_def
    // NOTE: the '% 4' and '/ 4' are to get the row and column of which grid
    // the digit is in the sprite map. Digits start at row 1 column 0 with the
    // value 0 and continue left to right.
    // In other words, this is to adjust to the png sprite manually
    r.hud_defs[hud_id].uv_pos = vec2_init((float) (digits[i] % 4), (float) (1 + digits[i] / 4));
    
    vertex_t hud_vertices[num_hud_rect_vertices];
    hud_init_rect(hud_vertices, &r.hud_defs[hud_id]);
    r_sub_mesh(&r.hud_mesh, hud_vertices, hud_id * num_hud_rect_vertices, num_hud_rect_vertices);
  }
}

void r_hud_update_round_time()
{
  int rt = cg.round_time;
  
  // round_time can be negative when waiting to load the next round. Clamp the
  // round_time to 0 if this is the case.
  if (rt < 0)
    rt = 0;
  
  int secs = rt / 1000;
  int mins = secs / 60;
  
  // Get each invidiaul digit
  int digits[] = {
    (mins % 100) / 10,
    (mins % 10) / 1,
    (secs % 60) / 10,
    (secs % 10) / 1
  };
  
  for (int i = 0; i < 4; i++) {
    r_hud_t hud_id = HUD_ROUND_TIME_DIGIT_0 + i;
    
    // see r_hud_health_update
    // TODO: perhaps there should be a label type element. However, as this
    // feature is used only twice, I'm leaving it like this for now.
    r.hud_defs[hud_id].uv_pos = vec2_init((float) (digits[i] % 4), (float) (1 + digits[i] / 4));
    
    vertex_t hud_vertices[num_hud_rect_vertices];
    hud_init_rect(hud_vertices, &r.hud_defs[hud_id]);
    r_sub_mesh(&r.hud_mesh, hud_vertices, hud_id * num_hud_rect_vertices, num_hud_rect_vertices);
  }
}

void r_render_hud()
{
  glDisable(GL_DEPTH_TEST);
  glUseProgram(r.hud_shader.program);
  glBindTexture(GL_TEXTURE_2D, r.hud_texture);
  
  r_hud_update_health();
  r_hud_update_round_time();
  r_draw_mesh(&r.hud_mesh);
}

bool r_init_hud_shader()
{
  // HUD shader
  // NOTE: because the HUD mesh is just a collection of rectangles fixed on the
  // screen, the shader uses no matrix transformations and instead relies on
  // the vertices loaded in to be already correct.
  
  static const char *src_vertex = ""
    "#version 300 es\n"
    "layout(location = 0) in vec3 v_pos;\n"
    "layout(location = 1) in vec3 v_normal;\n"
    "layout(location = 2) in vec2 v_uv;\n"
    "out vec2 vs_uv;\n"
    "void main() {\n"
    " vs_uv = v_uv;\n"
    " gl_Position = vec4(v_pos, 1.0);\n"
    "}";

  static const char *src_fragment = ""
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 frag_color;\n"
    "in vec4 vs_pos;\n"
    "in vec2 vs_uv;\n"
    "uniform sampler2D sampler;\n"
    "void main() {\n"
    " vec4 color = texture(sampler, vs_uv);\n"
    " frag_color = color;\n"
    "}";
  
  if (!gl_load_shader(&r.hud_shader.program, src_vertex, src_fragment)) {
    log_printf(LOG_ERROR, "r_init_cg_shader(): failed to load shader");
    return false;
  }
  
  return true;
}
