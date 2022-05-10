#include "r_local.h"

void r_light_init()
{
  glGenBuffers(1, &r.light_block);

  glBindBuffer(GL_UNIFORM_BUFFER, r.light_block);
  glBufferData(GL_UNIFORM_BUFFER, MAX_LIGHTS * sizeof(light_t), NULL, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, r.light_block);
  
  light_t lights[32];
  
  memset(lights, 0, sizeof(lights));
  
  for (int i = 0; i < 16; i++) {
    vec4_t color;
    if (i % 2 == 0)
      color = vec4_init(1.0f, 0.1f, 0.5f, 1.0f);
    else
      color = vec4_init(0.5f, 0.2f, 0.8f, 1.0f);
    
    lights[i] = (light_t) {
      .pos = { (float) (rand() % 40 - 20), (float) (4 + rand() % 5), (float) (rand() % 40 - 20) },
      .intensity = (float) (4 + rand() % 5),
      .color = color
    };
  }
  
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lights), &lights);
}
