#version 300 es
precision mediump float;

layout (location = 0) out vec4 frag_color;

in vec3 vs_pos;
in vec3 vs_normal;
in vec3 vs_frag_pos;
in vec2 vs_uv;

struct light_t {
 vec3 pos;
 float intensity;
 vec4 color;
};

layout (std140) uniform block {
 light_t lights[32];
};

uniform sampler2D   u_sampler;
uniform samplerCube u_skybox;
uniform bool        u_glow;
uniform vec3        u_view_pos;

void main() {
  vec3 ambient_light = vec3(0.1, 0.1, 0.1);
  
  vec3 light = ambient_light;
  
  float ratio = 1.00 / 2.42;
  
  vec3 I = normalize(vs_frag_pos - u_view_pos);
  vec3 R = refract(I, normalize(vs_normal), ratio);
  
  if (!u_glow) {
    for (int i = 0; i < 32; i++) {
      if (lights[i].intensity <= 0.0)
        continue;
      
      vec3 delta_pos = lights[i].pos - vs_pos;
      vec3 light_dir = normalize(delta_pos);
      vec3 view_dir = normalize(u_view_pos - vs_frag_pos);
      vec3 reflect_dir = reflect(-light_dir, vs_normal);
      
      float spec = 0.5 * pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);
      float delta_dist = length(delta_pos);
      float diffuse = max(dot(vs_normal, light_dir), 0.0);
      
      float attentuation = lights[i].intensity / (1.0 + 4.0 * delta_dist + 0.4 * delta_dist * delta_dist);
      float intensity = (diffuse + spec) * attentuation;
      light += lights[i].color.xyz * intensity;
    }
    
    vec3 color = texture(u_sampler, vs_uv).rgb + 0.5 * max(dot(vs_normal, vec3(0.0, 1.0, 0.0)), 0.1) * texture(u_skybox, R).rgb;
  
    frag_color = vec4(color, 1.0) * vec4(light, 1.0);
  } else {
    frag_color = vec4(10, 10, 10, 1.0);
  }
}
