#include "r_local.h"

static const vertex_t cube_vertices[] = {
  
  // Right
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Left
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { -1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Up
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { +1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Down
  { .pos = { -1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Front
  { .pos = { -1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { -1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, +1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, +1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  
  // Back
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { -1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
  { .pos = { +1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
  { .pos = { -1.0f, +1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
  { .pos = { +1.0f, -1.0f, -1.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
};

static const int num_cube_vertices = sizeof(cube_vertices) / sizeof(vertex_t);

bool r_skybox_init()
{
  glGenTextures(1, &r.skybox_texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, r.skybox_texture);
  
  static const char *faces[] = {
    "assets/skybox/right.png",
    "assets/skybox/left.png",
    "assets/skybox/up.png",
    "assets/skybox/down.png",
    "assets/skybox/back.png",
    "assets/skybox/front.png",
  };
  
  for (int i = 0; i < 6; i++) {
    SDL_Surface *bitmap = IMG_Load(faces[i]);
    
    if (!bitmap) {
      log_printf(LOG_ERROR, "r_skybox_init(): could not load %s", faces[i]);
      return false;
    }
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, bitmap->w, bitmap->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap->pixels);
    
    SDL_FreeSurface(bitmap);
  }
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  
  const char *src_vertex = file_read_all("assets/shader/skybox.vs");
  const char *src_fragment = file_read_all("assets/shader/skybox.fs");

  if (!gl_load_shader(&r.skybox_shader, src_vertex, src_fragment)) {
    log_printf(LOG_ERROR, "r_skybox_init(): failed to load shader");
    return false;
  }
  
  r.skybox_ul_mvp = glGetUniformLocation(r.skybox_shader, "u_mvp");
  
  if (!r_load_mesh(&r.skybox_mesh, cube_vertices, num_cube_vertices)) {
    log_printf(LOG_ERROR, "r_skybox_init(): failed to load skybox mesh");
    return false;
  }
  
  return true;
}

void r_skybox_render()
{
  glDepthMask(GL_FALSE);
  glUseProgram(r.skybox_shader);
  
  quat_t inverse_view_angle = quat_conjugate(cg.bg.transform[cg.ent_client].rotation);
  mat4x4_t rotation_matrix = mat4x4_init_rotation(inverse_view_angle);
  mat4x4_t view_projection_matrix = mat4x4_mul(rotation_matrix, r.projection_matrix);
  
  glUniformMatrix4fv(r.skybox_ul_mvp, 1, GL_FALSE, view_projection_matrix.m);
  glBindTexture(GL_TEXTURE_CUBE_MAP, r.skybox_texture);
  
  r_draw_mesh(&r.skybox_mesh);
  glDepthMask(GL_TRUE);
}
