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
  constructor(name, faces, mtl_id)
  {
    this.name = name;
    this.faces = faces;
    this.mtl_id = mtl_id;
  }
};

export class obj_entity_t {
  constructor(name, pos)
  {
    this.name = name;
    this.pos = pos;
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
  constructor(objects, entities, mtls)
  {
    this.objects = objects;
    this.entities = entities;
    this.mtls = mtls;
  }
};

function mtllib_parse(str_path)
{
  const file = fs.readFileSync(str_path).toString();
  const lines = file.split('\n');
  
  let mtl_name = "";
  let map_Kd = "";
  
  const mtls = [];
  
  for (const line of lines) {
    const args = line.split(' ').filter((x) => x.length > 0);
    
    if (args[0] == "newmtl") {
      if (mtl_name.length > 0) {
        const mtl = new obj_mtl_t(mtl_name, map_Kd);
        mtls.push(mtl);
      }
      
      const name = args[1];
      mtl_name = name;
    } else if (args[0] == "map_Kd") {
      map_Kd = args[1];
    }
  }
  
  if (mtl_name.length > 0) {
    const mtl = new obj_mtl_t(mtl_name, map_Kd);
    mtls.push(mtl);
  }
  
  return mtls;
}

export function obj_parse(str_path)
{
  const dir = path.parse(str_path).dir;
  
  const file = fs.readFileSync(str_path).toString();
  const lines = file.split('\n');
  
  let o_name = "";
  let v_buf = [];
  let vt_buf = [];
  let vn_buf = [];
  let f_buf = [];
  let pos_buf = [];
  let mtl_id;
  
  let mtls = [];
  
  const objects = [];
  const entities = [];
  
  for (const line of lines) {
    const args = line.split(' ').filter((x) => x.length > 0);
    
    if (args[0] == "mtllib") {
      const mtls_path = path.join(dir, args[1]);
      mtls = mtllib_parse(mtls_path);
    } if (args[0] == "o") {
      if (f_buf.length > 0)
        objects.push(new obj_object_t(o_name, f_buf, mtl_id));
      else if (pos_buf.length == 1)
        entities.push(new obj_entity_t(o_name, v_buf[0]));
      
      f_buf = [];
      pos_buf = [];
      
      o_name = args[1];
    } else if (args[0] == "usemtl") {
      const mtl_name = args[1];
      for (let i = 0; i < mtls.length; i++) {
        if (mtl_name == mtls[i].name)
          mtl_id = i;
      }
    } else if (args[0] == "v") {
      const pos = new vec3_t(
          parseFloat(args[1]),
          parseFloat(args[2]),
          parseFloat(args[3]));
      v_buf.push(pos);
      pos_buf.push(pos);
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
  
  if (f_buf.length > 0)
    objects.push(new obj_object_t(o_name, f_buf, mtl_id));
  else if (pos_buf.length == 1)
    entities.push(new obj_entity_t(o_name, pos_buf[0]));
  
  return new obj_t(objects, entities, mtls);
}
