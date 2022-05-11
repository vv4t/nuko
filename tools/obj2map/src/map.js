"use strict";

import path from "path";
import { vec2_t, vec3_t } from "./math.js";

export class map_brush_type {
  static BRUSH_EMPTY = 0;
  static BRUSH_SOLID = 1;
};

export class map_brush_mtl {
  static MTL_GRASS    = 0;
  static MTL_CONCRETE = 1;
  static MTL_BRICK    = 2;
  static MTL_BUILDING = 3;
};

export class map_light_t {
  constructor(pos, intensity, color)
  {
    this.pos = pos;
    this.intensity = intensity;
    this.color = color;
  }
}

export class map_vertex_t {
  constructor(pos, normal, uv)
  {
    this.pos = pos;
    this.normal = normal;
    this.uv = uv;
  }
};

export class map_face_t {
  constructor(vertices, normal)
  {
    this.vertices = vertices;
    this.normal = normal;
  }
};

export class map_brush_t {
  constructor(faces, mtl_id)
  {
    this.faces = faces;
    this.mtl_id = mtl_id;
  }
};

export class map_mtl_t {
  constructor(name)
  {
    this.name = name;
  }
};

export class map_t {
  constructor(brushes, lights, mtls)
  {
    this.brushes = brushes;
    this.lights = lights;
    this.mtls = mtls;
  }
};

export function map_from_obj(obj)
{
  const brushes = [];
  const lights = [];
  const mtls = [];
  
  for (const obj_mtl of obj.mtls) {
    const mtl_name = path.parse(obj_mtl.map_Kd).name;
    const mtl = new map_mtl_t(mtl_name);
    
    mtls.push(mtl);
  }
  
  for (const object of obj.objects) {
    const faces = [];
    const mtl_id = object.mtl_id;
    
    for (const face of object.faces) {
      const vertices = [];
      const normal = face.vertices[0].normal.normalize();
      
      for (const vertex of face.vertices) {
        const map_vertex = new map_vertex_t(vertex.pos, normal, vertex.uv);
        vertices.push(map_vertex);
      }
      
      const map_face = new map_face_t(vertices, normal);
      faces.push(map_face);
    }
    
    const map_brush = new map_brush_t(faces, mtl_id);
    brushes.push(map_brush);
  }
  
  for (const entity of obj.entities) {
    const args = entity.name.split("_");
    
    if (args[0].startsWith("light")) {
      const intensity = parseFloat(args[1]);
      const color = new vec3_t(
        parseFloat(args[2]),
        parseFloat(args[3]),
        parseFloat(args[4]));
      
      lights.push(new map_light_t(entity.pos, intensity, color));
    }
  }
  
  const map = new map_t(brushes, lights, mtls);
  
  return map;
}
