#include "r_local.h"

bool r_hdr_init()
{
  glGenFramebuffers(1, &r.hdr_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, r.hdr_fbo);
  
  glGenTextures(1, &r.color_buffer);
  
  glBindTexture(GL_TEXTURE_2D, r.color_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r.color_buffer, 0);

#ifdef __EMSCRIPTEN__
  glGenTextures(1, &r.hdr_rbo);
  glBindTexture(GL_TEXTURE_2D, r.hdr_rbo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, r.hdr_rbo, 0);
#else
  glGenRenderbuffers(1, &r.hdr_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, r.hdr_rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, r.hdr_rbo);
#endif
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  const char *src_vertex = file_read_all("assets/shader/hud.vs");
  const char *src_fragment = file_read_all("assets/shader/hdr.fs");

  if (!gl_load_shader(&r.hdr_shader, src_vertex, src_fragment)) {
    log_printf(LOG_ERROR, "r_hdr_int(): failed to load shader");
    return false;
  }
  
  const vertex_t quad_vertices[] = {
    { .pos = { -1.0f, -1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } },
    { .pos = { -1.0f, +1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 0.0f, 1.0f } },
    { .pos = { +1.0f, +1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 1.0f } },
    { .pos = { +1.0f, -1.0f, 0.0f }, .normal = { 0.0f, 0.0f, -1.0f }, .uv = { 1.0f, 0.0f } }
  };
  
  if (!r_load_mesh(&r.screen_mesh, quad_vertices, 6)) {
    log_printf(LOG_ERROR, "r_hdr_init(): failed to allocate screen mesh vertices");
    return false;
  }
  
  return true;
}

void r_hdr_begin()
{
  glBindFramebuffer(GL_FRAMEBUFFER, r.hdr_fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void r_hdr_end()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void r_hdr_draw()
{
  glDisable(GL_DEPTH_TEST);
  
  glUseProgram(r.hdr_shader);
  glBindTexture(GL_TEXTURE_2D, r.color_buffer);
  r_draw_mesh(&r.screen_mesh);
}
