"use strict";

import { vertex_t } from "./common/vertex.js";
import { material_t, brush_t, face_t } from "./common/map.js";
import { vec2_t, vec3_t } from "./common/math.js";
import { asset_load_json } from "./asset.js";

export function map_load(path, on_loaded)
{
  asset_load_json(path, function(map) {
    on_loaded(new map_handle_t(map));
  });
}

export class map_handle_t {
  constructor(map)
  {
    this.brushes = [];
    
    for (const brush of map.brushes) {
      const faces = [];
      const material = new material_t(brush.material.path);
      
      for (const face of brush.faces) {
        const vertices = [];
        const normal = new vec3_t(face.normal.x, face.normal.y, face.normal.z);
        
        for (const vertex of face.vertices) {
          const pos = new vec3_t(vertex.pos.x, vertex.pos.y, vertex.pos.z);
          const uv = new vec2_t(vertex.uv.x, vertex.uv.y);
          
          vertices.push(new vertex_t(pos, uv));
        }
        
        faces.push(new face_t(vertices, normal));
      }
      
      this.brushes.push(new brush_t(faces, material));
    }
  }
}
