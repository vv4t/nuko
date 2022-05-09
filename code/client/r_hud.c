#include "r_local.h"

static const vertex_t hud_rect[] = {
  { .pos = { 0.0f, 0.0f, 0.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { 0.0f, 1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { 1.0f, 0.0f, 0.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { 0.0f, 1.0f, 0.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { 1.0f, 1.0f, 0.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { 1.0f, 0.0f, 0.0f }, .uv = { 1.0f, 0.0f } }
};

static const int num_hud_rect_vertices = sizeof(hud_rect) / sizeof(vertex_t);

void hud_init_rect(vertex_t *vertices, const hud_def_t *hud_def)
{
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
      vec3_init(hud_def->scr_pos.x, hud_def->scr_pos.y, 0.0f),
      vec3_mul(
        vec3_add(
          hud_rect[i].pos,
          align_pos),
        vec3_init(hud_def->scr_size.x * ASPECT_RATIO, hud_def->scr_size.y, 1.0f)));
    
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
  
  if (!gl_load_texture(&r.hud_texture, "assets/tex/hud.png")) {
    log_printf(LOG_ERROR, "r_load_materials(): failed to load texture 'assets/tex/hud.png'");
    return false;
  }
  
  return true;
}

bool r_init_hud_mesh()
{
  r.hud_defs[HUD_CROSSHAIR].hud_align   = HUD_ALIGN_CENTER;
  r.hud_defs[HUD_CROSSHAIR].scr_pos     = vec2_init(0.0f, 0.0f);
  r.hud_defs[HUD_CROSSHAIR].scr_size    = vec2_init(0.05f, 0.05f);
  r.hud_defs[HUD_CROSSHAIR].uv_pos      = vec2_init(0.0f, 0.0f);
  r.hud_defs[HUD_CROSSHAIR].uv_size     = vec2_init(1.0f, 1.0f);
  
  if (!r_new_mesh(&r.hud_mesh, MAX_HUDS * num_hud_rect_vertices)) {
    log_printf(LOG_ERROR, "r_init_hud_mesh(): failed to allocate hud mesh");
    return false;
  }
  
  for (int i = 0; i < MAX_HUDS; i++) {
    vertex_t hud_vertices[num_hud_rect_vertices];
    hud_init_rect(hud_vertices, &r.hud_defs[i]);
    r_sub_mesh(&r.hud_mesh, hud_vertices, i * num_hud_rect_vertices, num_hud_rect_vertices);
  }
  
  return true;
}

bool r_init_hud_shader()
{
  static const char *src_vertex = ""
    "#version 300 es\n"
    "layout(location = 0) in vec3 v_pos;\n"
    "layout(location = 1) in vec2 v_uv;\n"
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
  
  r.hud_shader.ul_mvp = glGetUniformLocation(r.hud_shader.program, "u_mvp");
  
  return true;
}

void r_render_hud()
{
  glUseProgram(r.hud_shader.program);
  glBindTexture(GL_TEXTURE_2D, r.hud_texture);
  
  r_draw_mesh(&r.hud_mesh);
}
