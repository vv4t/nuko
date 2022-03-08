"use strict";

import { gl } from "./gl.js";
import { vertex_t } from "./vertex.js";
import { mesh_pool_t } from "./mesh-pool.js";
import { basic_shader_t } from "./basic-shader.js";

import { screen } from "../screen.js";
import { vec3_t, mat4_t, quat_t } from "../common/math.js";

export class renderer_t {
  constructor(cgame)
  {
    gl.clearColor(0.0, 0.5, 1.0, 1.0);
    
    this.cgame = cgame;
    this.mesh_pool = new mesh_pool_t(1024);
    this.basic_shader = new basic_shader_t();
    
    const FOV = 90 * Math.PI / 180.0;
    const aspect_ratio = screen.height / screen.width;
    
    this.view_matrix = mat4_t.init_identity();
    this.projection_matrix = mat4_t.init_perspective(aspect_ratio, FOV, 0.1, 100);
    
    this.basic_shader.bind();
    
    const vertices = [
      new vertex_t(new vec3_t(0, 0, 0)),
      new vertex_t(new vec3_t(0, 1, 0)),
      new vertex_t(new vec3_t(1, 0, 0)),
    ];
    
    this.mesh = this.mesh_pool.new_mesh(vertices);
  }
  
  setup_view_matrix()
  {
    const view_origin = this.cgame.c_transform[this.cgame.player].pos;
    const view_angle = this.cgame.c_transform[this.cgame.player].rot;
    
    const inverted_origin = view_origin.mulf(-1);
    const inverted_angle = view_angle.conjugate();
    
    const translation_matrix = mat4_t.init_translation(inverted_origin);
    const rotation_matrix = mat4_t.init_rotation(inverted_angle);
    
    this.view_matrix = translation_matrix.mul(rotation_matrix);
  }
  
  render()
  {
    gl.clear(gl.COLOR_BUFFER_BIT);
    
    this.setup_view_matrix();
    
    const theta = performance.now() * 0.001;
    
    const transform = mat4_t.init_identity();// mat4_t.init_rotation(quat_t.init_rotation(new vec3_t(0, 0, 1), theta));
    const mvp = transform.mul(this.view_matrix).mul(this.projection_matrix);
    
    this.basic_shader.set_mvp(mvp);
    
    this.mesh.draw();
  } 
};
