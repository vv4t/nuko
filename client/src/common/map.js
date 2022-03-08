"use strict";

export class face_t {
  constructor(vertices, normal)
  {
    if (vertices.length != 3)
      throw new Error("face_t::constructor(): not a triangle");
    
    this.vertices = vertices;
    this.normal = normal;
  }
}

export class brush_t {
  constructor(faces)
  {
    this.faces = faces;
  }
}

export class bspnode_t {
  constructor(type, plane, behind, ahead)
  {
    this.type = type;
    this.plane = plane;
    this.behind = behind;
    this.ahead = ahead;
  }
}
