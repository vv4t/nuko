#version 300 es
precision mediump float;

out vec4 frag_color;

in vec4 vs_pos;
in vec2 vs_uv;

uniform sampler2D sampler;

void main() {
 vec4 color = texture(sampler, vs_uv);
 frag_color = color;
}
