"use strict";

import { gl } from "./gl.js";
import { mesh_pool_t } from "./mesh-pool.js";
import { basic_shader_t } from "./basic-shader.js";

import { vec3_t, mat4_t } from "../common/math.js";
import { vertex_t } from "./vertex.js";

export class renderer_t {
  constructor()
  {
    gl.clearColor(0.0, 0.5, 1.0, 1.0);
    
    this.mesh_pool = new mesh_pool_t(1024);
    this.basic_shader = new basic_shader_t();
    
    this.basic_shader.bind();
    
    const identity = mat4_t.init_identity();
    
    this.basic_shader.set_mvp(identity);
    
    const vertices = [
      new vertex_t(new vec3_t(0, 0, 0)),
      new vertex_t(new vec3_t(0, 1, 0)),
      new vertex_t(new vec3_t(1, 0, 0)),
    ];
    
    this.mesh = this.mesh_pool.new_mesh(vertices);
  }
  
  render()
  {
    gl.clear(gl.COLOR_BUFFER_BIT);
    
    this.mesh.draw();
  } 
};
