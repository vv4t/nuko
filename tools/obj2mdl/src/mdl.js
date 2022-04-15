"use strict";

import path from "path";

export class mdl_vertex_t {
  constructor(pos, uv)
  {
    this.pos = pos;
    this.uv = uv;
  }
};

export class mdl_mtl_t {
  constructor(name)
  {
    this.name = name;
  }
}

export class mdl_object_t {
  constructor(vertices, mtl_id)
  {
    this.vertices = vertices;
    this.mtl_id = mtl_id;
  }
};

export class mdl_t {
  constructor(objects, mtls)
  {
    this.objects = objects;
    this.mtls = mtls;
  }
};

export function mdl_from_obj(obj)
{
  const objects = [];
  const mtls = [];
  
  for (const obj_mtl of obj.mtls) {
    const mtl_name = path.parse(obj_mtl.map_Kd).name;
    const mtl = new mdl_mtl_t(mtl_name);
    
    mtls.push(mtl);
  }
  
  for (const object of obj.objects) {
    const vertices = [];
    const mtl_id = object.mtl_id;
    
    for (const face of object.faces) {
      for (const vertex of face.vertices) {
        const mdl_vertex = new mdl_vertex_t (vertex.pos, vertex.uv);
        vertices.push(mdl_vertex);
      }
    }
    
    const mdl_object = new mdl_object_t(vertices, mtl_id);
    
    objects.push(mdl_object);
  }
  
  const mdl = new mdl_t(objects, mtls);
  
  return mdl;
}
