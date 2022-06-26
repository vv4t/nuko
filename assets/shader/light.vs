#version 300 es

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

out vec3 vs_pos;
out vec3 vs_normal;
out vec3 vs_frag_pos;
out vec2 vs_uv;

uniform mat4 u_mvp;
uniform mat4 u_model;

void main() {
 vs_pos = vec3(u_model * vec4(v_pos, 1.0));
 vs_normal = mat3(transpose(inverse(u_model))) * v_normal;
 vs_uv = v_uv;
 vs_frag_pos = vec3(u_model * vec4(v_pos, 1.0));
 gl_Position = u_mvp * vec4(v_pos, 1.0);
}
