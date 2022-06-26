#include "r_local.h"

bool r_light_init()
{
  r_init_light_block();
  
  if (!r_init_light_shader()) {
    log_printf(LOG_ERROR, "r_light_init(): failed to load light shader");
    return false;
  }
  
  return true;
}

void r_set_light(r_light_t light_id, vec3_t pos, float intensity, vec4_t color)
{
  light_t light = {
    .pos = pos,
    .intensity = intensity,
    .color = color
  };
  
  glBufferSubData(GL_UNIFORM_BUFFER, light_id * sizeof(light_t), sizeof(light_t), &light);
}

void r_light_reset()
{
  memset(r.light_active, 0, sizeof(r.light_active));
  
  // The shader tests if the intensity is 0 to see if a light is allocated.
  // Therefore, all the lights need to be zero'd to deallocate them all.
  light_t lights[32];
  memset(lights, 0, sizeof(lights));
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lights), &lights);
}

void r_remove_light(r_light_t light)
{
  r.light_active[light] = false;
  r_set_light(light, vec3_init(0.0f, 0.0f, 0.0f), 0.0f, vec4_init(0.0f, 0.0f, 0.0f, 1.0f));
}

r_light_t r_new_light()
{
  // Find the first inactive light
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (!r.light_active[i]) {
      r.light_active[i] = true;
      return i;
    }
  }
  
  log_printf(LOG_WARNING, "r_new_light(): ran out of lights");
  
  return -1;
}

// NOTE: though it is called a 'uniform buffer', for simplicity, it is called
// 'block' throughout the project.
void r_init_light_block()
{
  glGenBuffers(1, &r.light_shader.block);

  glBindBuffer(GL_UNIFORM_BUFFER, r.light_shader.block);
  glBufferData(GL_UNIFORM_BUFFER, MAX_LIGHTS * sizeof(light_t), NULL, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, r.light_shader.block);
}

bool r_init_light_shader()
{
  const char *src_vertex = file_read_all("assets/shader/light.vs");
  const char *src_fragment = file_read_all("assets/shader/light.fs");

  if (!gl_load_shader(&r.light_shader.program, src_vertex, src_fragment)) {
    log_printf(LOG_ERROR, "r_init_cg_shader(): failed to load shader");
    return false;
  }
  
  r.light_shader.ul_mvp = glGetUniformLocation(r.light_shader.program, "u_mvp");
  r.light_shader.ul_model = glGetUniformLocation(r.light_shader.program, "u_model");
  r.light_shader.ul_glow = glGetUniformLocation(r.light_shader.program, "u_glow");
  r.light_shader.ul_view_pos = glGetUniformLocation(r.light_shader.program, "u_view_pos");
  r.light_shader.ul_sampler = glGetUniformLocation(r.light_shader.program, "u_sampler");
  r.light_shader.ul_skybox = glGetUniformLocation(r.light_shader.program, "u_skybox");
  
  glUseProgram(r.light_shader.program);
  glUniform1i(r.light_shader.ul_sampler, 0);
  glUniform1i(r.light_shader.ul_skybox, 1);
  
  return true;
}
