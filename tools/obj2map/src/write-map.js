"use strict";

import fs from "fs";
import { bsp_gen } from "./bsp.js";
import { map_t } from "./map.js";
import { vec2_t, vec3_t, plane_t } from "./math.js"

class sizeof {
  static U8       = 1;
  static U16      = 2;
  static U32      = 4;
  static F32      = 4;
  
  static VEC2_T   = 2 * sizeof.F32;
  static VEC3_T   = 3 * sizeof.F32;
  
  static LUMP_T   =  sizeof.U32 + sizeof.U32;
};

class map_file_lump {
  static VERTICES       = 0;
  static BSP_NODES      = 1;
  static BRUSH_GROUPS   = 2;
};

class map_file_mtl_t {
  constructor(name)
  {
    this.name = name;
  }
};

class map_file_bsp_node_t {
  constructor(type, plane, behind, ahead)
  {
    this.type = type;
    this.plane = plane;
    this.behind = behind;
    this.ahead = ahead;
  }
};

class map_file_brush_group_t {
  constructor(mtl, vertexofs, vertexlen)
  {
    this.mtl = mtl;
    this.vertexofs = vertexofs;
    this.vertexlen = vertexlen;
  }
};

class lump_t {
  constructor(fileofs, filelen)
  {
    this.fileofs = fileofs;
    this.filelen = filelen;
  }
};

export function write_map(map, path)
{
  const write = new write_t();
  
  const [vertices, brush_groups] = flatten_brushes(map.brushes);
  const bsp_nodes = flatten_bsp_node_R(bsp_gen(map), 0);
  
  write.seek(3 * sizeof.LUMP_T);
  
  const vertices_fileofs = write.tell();
  for (const vertex of vertices)
    write.write_vertex(vertex);
  const vertices_filelen = write.tell() - vertices_fileofs;
  
  const bsp_nodes_fileofs = write.tell();
  for (const bsp_node of bsp_nodes)
    write.write_bsp_node(bsp_node);
  const bsp_nodes_filelen = write.tell() - bsp_nodes_fileofs;
  
  const brush_group_fileofs = write.tell();
  for (const brush_group of brush_groups)
    write.write_brush_group(brush_group);
  const brush_group_filelen = write.tell() - brush_group_fileofs;
  
  const lump_vertices = new lump_t(vertices_fileofs, vertices_filelen);
  const lump_bsp_nodes = new lump_t(bsp_nodes_fileofs, bsp_nodes_filelen);
  const lump_brush_group = new lump_t(brush_group_fileofs, brush_group_filelen);
  
  write.seek(0);
  write.write_lump(lump_vertices);
  write.write_lump(lump_bsp_nodes);
  write.write_lump(lump_brush_group);
  
  fs.writeFileSync(path, Buffer.from(write.data()));
}

function flatten_brushes(brushes)
{
  const sorted_brushes = brushes.slice();
  
  const vertices = [];
  const brush_groups = [];
  
  let vertexofs = 0;
  let brushofs = 0;
  let brushend = brushofs;
  
  while (brushend != sorted_brushes.length) {
    vertices.push(...flatten_brush(sorted_brushes[brushend]));
    
    const mtl = sorted_brushes[brushend].mtl;
    
    for (let j = brushend + 1; j < sorted_brushes.length; j++) {
      if (mtl == sorted_brushes[j].mtl) {
        brushend++;
        
        const tmp = sorted_brushes[brushend];
        sorted_brushes[brushend] = sorted_brushes[j];
        sorted_brushes[j] = tmp;
        
        vertices.push(...flatten_brush(sorted_brushes[brushend]));
      }
    }
    
    brushend++;
    
    const vertexlen = vertices.length - vertexofs;
    const brush_group = new map_file_brush_group_t(mtl, vertexofs, vertexlen);
    brush_groups.push(brush_group);
    
    vertexofs = vertices.length;
  }
  
  return [vertices, brush_groups];
}

function flatten_brush(brush)
{
  const vertices = [];
  for (const face of brush.faces) {
    for (const vertex of face.vertices) {
      vertices.push(vertex);
    }
  }
  return vertices;
}

function flatten_bsp_node_R(bsp_node, offset)
{
  const bsp_nodes = [];
  
  bsp_nodes.length = 1;
  
  let behind = -1;
  if (bsp_node.behind) {
    behind = offset + bsp_nodes.length;
    bsp_nodes.push(...flatten_bsp_node_R(bsp_node.behind, behind));
  }
  
  let ahead = -1;
  if (bsp_node.ahead) {
    ahead = offset + bsp_nodes.length;
    bsp_nodes.push(...flatten_bsp_node_R(bsp_node.ahead, ahead));
  }
  
  bsp_nodes[0] = new map_file_bsp_node_t(bsp_node.type, bsp_node.plane, behind, ahead);
  
  return bsp_nodes;
}

function align(x, align)
{
  return Math.floor(x / align);
}

class write_t {
  constructor()
  {
    this.b = new ArrayBuffer(8);
    this._reset();
    
    this.pos = 0;
    this.max_pos = this.pos;
  }
  
  data()
  {
    const new_b = new ArrayBuffer(this.max_pos + 1);
    const new_b_u8 = new Uint8Array(new_b);
    
    for (let i = 0; i < this.max_pos; i++)
      new_b_u8[i] = this.b_u8[i];
    
    return new_b;
  }
  
  _reset()
  {
    this.b_u8 = new Uint8Array(this.b);
    this.b_u16 = new Uint16Array(this.b);
    this.b_u32 = new Uint32Array(this.b);
    this.b_f32 = new Float32Array(this.b);
  }
  
  grow()
  {
    const new_b = new ArrayBuffer(2 * this.b.byteLength);
    new Uint8Array(new_b).set(this.b_u8);
    this.b = new_b;
    this._reset();
  }
  
  seek(pos)
  {
    this.pos = pos;
    
    if (this.pos > this.max_pos)
      this.max_pos = this.pos;
    
    while (pos >= this.b.byteLength)
      this.grow();
  }
  
  tell()
  {
    return this.pos;
  }
  
  write_u8(u8)
  {
    this.b_u8[align(this.pos, sizeof.U8)] = u8|0;
    this.seek(this.pos + sizeof.U8);
  }
  
  write_u16(u16)
  {
    this.b_u16[align(this.pos, sizeof.U16)] = u16|0;
    this.seek(this.pos + sizeof.U16);
  }
  
  write_u32(u32)
  {
    this.b_u32[align(this.pos, sizeof.U32)] = u32|0;
    this.seek(this.pos + sizeof.U32);
  }
  
  write_f32(f32)
  {
    this.b_f32[align(this.pos, sizeof.F32)] = f32;
    this.seek(this.pos + sizeof.F32);
  }
  
  write_vec2(vec2)
  {
    this.write_f32(vec2.x);
    this.write_f32(vec2.y);
  }
  
  write_vec3(vec3)
  {
    this.write_f32(vec3.x);
    this.write_f32(vec3.y);
    this.write_f32(vec3.z);
  }
  
  write_plane(plane)
  {
    this.write_vec3(plane.normal);
    this.write_f32(plane.distance);
  }

  write_bsp_node(bsp_node)
  {
    this.write_u32(bsp_node.type);
    this.write_plane(bsp_node.plane);
    this.write_u32(bsp_node.behind);
    this.write_u32(bsp_node.ahead);
  }
  
  write_brush_group(brush_group)
  {
    this.write_u32(brush_group.mtl);
    this.write_u32(brush_group.vertexofs);
    this.write_u32(brush_group.vertexlen);
  }
  
  write_lump(lump)
  {
    this.write_u32(lump.fileofs);
    this.write_u32(lump.filelen);
  }

  write_vertex(vertex)
  {
    this.write_vec3(vertex.pos);
    this.write_vec2(vertex.uv);
  }
};
