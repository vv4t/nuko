"use strict";

import fs from "fs";
import path from "path";
import { vec2_t, vec3_t } from "./math.js";

export class obj_vertex_t {
  constructor(pos, uv, normal)
  {
    this.pos = pos;
    this.uv = uv;
    this.normal = normal;
  }
};

export class obj_face_t {
  constructor(vertices)
  {
    this.vertices = vertices;
  }
};

export class obj_object_t {
  constructor(faces, mtl)
  {
    this.faces = faces;
    this.mtl = mtl;
  }
};

export class obj_mtl_t {
  constructor(name, map_Kd)
  {
    this.name = name;
    this.map_Kd = map_Kd;
  }
};

export class obj_t {
  constructor(objects)
  {
    this.objects = objects;
  }
};

function mtllib_parse(str_path)
{
  const file = fs.readFileSync(str_path).toString();
  const lines = file.split('\n');
  
  let mtl_name = "";
  let map_Kd = "";
  
  const mtllib = {};
  
  for (const line of lines) {
    const args = line.split(' ').filter((x) => x.length > 0);
    
    if (args[0] == "newmtl") {
      if (mtl_name.length > 0) {
        const mtl = new obj_mtl_t(mtl_name, map_Kd);
        mtllib[mtl_name] = mtl;
      }
      
      const name = args[1];
      mtl_name = name;
    } else if (args[0] == "map_Kd") {
      map_Kd = args[1];
    }
  }
  
  if (mtl_name.length > 0) {
    const mtl = new obj_mtl_t(mtl_name, map_Kd);
    mtllib[mtl_name] = mtl;
  }
  
  return mtllib;
}

export function obj_parse(str_path)
{
  const dir = path.parse(str_path).dir;
  
  const file = fs.readFileSync(str_path).toString();
  const lines = file.split('\n');
  
  let v_buf = [];
  let vt_buf = [];
  let vn_buf = [];
  let f_buf = [];
  let mtl;
  
  let mtllib = {};
  
  const objects = [];
  
  for (const line of lines) {
    const args = line.split(' ').filter((x) => x.length > 0);
    
    if (args[0] == "mtllib") {
      const mtllib_path = path.join(dir, args[1]);
      mtllib = mtllib_parse(mtllib_path);
    } if (args[0] == "o") {
      if (f_buf.length > 0) {
        const object = new obj_object_t(f_buf, mtl);
        objects.push(object);
        f_buf = [];
      }
    } else if (args[0] == "usemtl") {
      const mtl_name = args[1];
      mtl = mtllib[mtl_name];
    } else if (args[0] == "v") {
      v_buf.push(
        new vec3_t(
          parseFloat(args[1]),
          parseFloat(args[2]),
          parseFloat(args[3])));
    } else if (args[0] == "vt") {
      vt_buf.push(
        new vec2_t(
          parseFloat(args[1]),
          1 - parseFloat(args[2])));
    } else if (args[0] == "vn") {
      vn_buf.push(
        new vec3_t(
          parseFloat(args[1]),
          parseFloat(args[2]),
          parseFloat(args[3])));
    } else if (args[0] == "f") {
      const vertices = [];
      
      for (let i = 1; i < 4; i++) {
        const face_data = args[i].split('/').map((x) => parseInt(x, 10));
        
        const v_id = face_data[0] - 1;
        const vt_id = face_data[1] - 1;
        const vn_id = face_data[2] - 1;
        
        const vertex = new obj_vertex_t(v_buf[v_id], vt_buf[vt_id], vn_buf[vn_id]);
        
        vertices.push(vertex);
      }
      
      const face = new obj_face_t(vertices);
      
      f_buf.push(face);
    }
  }
  
  if (f_buf.length > 0) {
    const object = new obj_object_t(f_buf, mtl);
    objects.push(object);
    f_buf = [];
  }
  
  return new obj_t(objects);
}
