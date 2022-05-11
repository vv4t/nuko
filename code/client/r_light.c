#include "r_local.h"

bool r_light_init()
{
  r_init_light_block();
  
  if (!r_init_light_shader()) {
    log_printf(LOG_ERROR, "r_cg_init(): failed to load cg shader");
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
  for (int i = 0; i < MAX_LIGHTS; i++) {
    if (!r.light_active[i]) {
      r.light_active[i] = true;
      return i;
    }
  }
  
  log_printf(LOG_WARNING, "r_new_light(): ran out of lights");
  
  return -1;
}

void r_init_light_block()
{
  glGenBuffers(1, &r.light_shader.block);

  glBindBuffer(GL_UNIFORM_BUFFER, r.light_shader.block);
  glBufferData(GL_UNIFORM_BUFFER, MAX_LIGHTS * sizeof(light_t), NULL, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, r.light_shader.block);
}

bool r_init_light_shader()
{
  static const char *src_vertex = ""
    "#version 300 es\n"
    "layout(location = 0) in vec3 v_pos;\n"
    "layout(location = 1) in vec3 v_normal;\n"
    "layout(location = 2) in vec2 v_uv;\n"
    "out vec3 vs_pos;\n"
    "out vec3 vs_normal;\n"
    "out vec2 vs_uv;\n"
    "uniform mat4 u_mvp;"
    "uniform mat4 u_model;"
    "void main() {\n"
    " vs_pos = vec3(u_model * vec4(v_pos, 1.0));\n"
    " vs_normal = v_normal;\n"
    " vs_uv = v_uv;\n"
    " gl_Position = u_mvp * vec4(v_pos, 1.0);\n"
    "}";

  static const char *src_fragment = ""
  "#version 300 es\n"
  "precision mediump float;\n"
  "\n"
  "out vec4 frag_color;\n"
  "in vec3 vs_pos;\n"
  "in vec3 vs_normal;\n"
  "in vec2 vs_uv;\n"
  "\n"
  "struct light_t {\n"
  " vec3 pos;\n"
  " float intensity;\n"
  " vec4 color;\n"
  "};\n"
  "\n"
  "layout (std140) uniform block {\n"
  " light_t lights[32];\n"
  "};\n"
  "\n"
  "uniform sampler2D sampler;\n"
  "\n"
  "void main() {\n"
  "  vec3 ambient_light = vec3(0.1, 0.1, 0.1);\n"
  "  \n"
  "  vec3 light = ambient_light;\n"
  "  \n"
  "  for (int i = 0; i < 32; i++) {\n"
  "    if (lights[i].intensity <= 0.0)\n"
  "      continue;\n"
  "    \n"
  "    vec3 delta_pos = lights[i].pos - vs_pos;\n"
  "    vec3 light_dir = normalize(delta_pos);\n"
  "    float delta_dist = length(delta_pos);\n"
  "    float diffuse = max(dot(vs_normal, light_dir), 0.0);\n"
  "    float ambience = 0.1;\n"
  "    float attentuation = lights[i].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);\n"
  "    float intensity = diffuse * attentuation;\n"
  "    light += lights[i].color.xyz * clamp(intensity, 0.0, 1.0);\n"
  "  }\n"
  "  \n"
  "  vec4 color = texture(sampler, vs_uv);\n"
  "  frag_color = vec4(light, 1.0) * color;\n"
  "}";

  if (!gl_load_shader(&r.light_shader.program, src_vertex, src_fragment)) {
    log_printf(LOG_ERROR, "r_init_cg_shader(): failed to load shader");
    return false;
  }
  
  r.light_shader.ul_mvp = glGetUniformLocation(r.light_shader.program, "u_mvp");
  r.light_shader.ul_model = glGetUniformLocation(r.light_shader.program, "u_model");
  
  return true;
}
