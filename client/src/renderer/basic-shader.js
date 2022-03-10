"use strict";

import { gl } from "./gl.js";
import { shader_t } from "./shader.js";

export class basic_shader_t extends shader_t {
  constructor()
  {
    /*
    const src_basic_vertex_shader = "" +
      "#version 300 es\n" +
      "layout(location = 0) in vec3 v_pos;\n" +
      "layout(location = 1) in vec2 v_uv;\n" +
      "uniform mat4 u_mvp;\n" +
      "out vec2 vs_uv;\n" +
      "void main() {\n" + 
      " vs_uv = vs_uv;\n" +
      " gl_Position = u_mvp * vec4(v_pos, 1);\n" +
      "}\n";

    const src_basic_fragment_shader = "" +
      "#version 300 es\n" + 
      "precision mediump float;\n" +
      "out vec4 frag_color;\n" +
      "in vec2 vs_uv;\n" +
      "uniform sampler2D sampler;\n" +
      "void main() {\n" +
      " vec4 color = vec4(1, 1, 1, 1);//texture(sampler, vs_uv);\n" +
      " if (color.w == 0.0)\n" +
      "  discard;\n" +
      " frag_color = color;\n" + 
      "}";
    */
    
    const src_basic_vertex_shader = "" +
      "#version 300 es\n" +
      "layout(location = 0) in vec3 v_pos;\n" +
      "uniform mat4 u_mvp;\n" +
      "out vec4 depth;\n" +
      "void main() {\n" + 
      " gl_Position = u_mvp * vec4(v_pos, 1);\n" +
      " depth = gl_Position;\n" +
      "}\n";

    const src_basic_fragment_shader = "" +
      "#version 300 es\n" + 
      "precision mediump float;\n" +
      "out vec4 frag_color;\n" +
      "in vec4 depth;\n" +
      "void main() {\n" +
      " float l = 0.1 + 0.9 / length(depth);\n" +
      " frag_color = vec4(l, l, l, 1);\n" + 
      "}";
    
    super(src_basic_vertex_shader, src_basic_fragment_shader);
    
    this.uloc_mvp = this.get_uniform_location("u_mvp");
  }
  
  set_mvp(mvp)
  {
    gl.uniformMatrix4fv(this.uloc_mvp, gl.FALSE, mvp.m);
  }
}
