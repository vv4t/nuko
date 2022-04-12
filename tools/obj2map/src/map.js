"use strict";

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

export class map_vertex_t {
  constructor(pos, uv)
  {
    this.pos = pos;
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
  constructor(faces, mtl)
  {
    this.faces = faces;
    this.mtl = mtl;
  }
};

export class map_t {
  constructor(brushes)
  {
    this.brushes = brushes;
  }
};

function to_brush_mtl(obj_mtl)
{
  switch (obj_mtl) {
  case "grass":
    return map_brush_mtl.MTL_GRASS;
  case "concrete":
    return map_brush_mtl.MTL_CONCRETE;
  case "brick":
    return map_brush_mtl.MTL_BRICK;
  case "building":
    return map_brush_mtl.MTL_BUILDING;
  }
}

export function map_from_obj(obj)
{
  const brushes = [];
  
  for (const object of obj.objects) {
    const faces = [];
    const mtl = to_brush_mtl(object.mtl.name);
    
    for (const face of object.faces) {
      const vertices = [];
      const normal = face.vertices[0].normal.normalize();
      
      for (const vertex of face.vertices) {
        const map_vertex = new map_vertex_t(vertex.pos, vertex.uv);
        vertices.push(map_vertex);
      }
      
      const map_face = new map_face_t(vertices, normal);
      faces.push(map_face);
    }
    
    const map_brush = new map_brush_t(faces, mtl);
    brushes.push(map_brush);
  }
  
  const map = new map_t(brushes);
  
  return map;
}
