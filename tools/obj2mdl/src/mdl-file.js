"use strict";

import fs from "fs";
import { mdl_t } from "./mdl.js";
import { vec2_t, vec3_t, plane_t } from "./math.js"

const MAX_STRING_SIZE = 32;

class sizeof {
  static U8       = 1;
  static U16      = 2;
  static U32      = 4;
  static F32      = 4;
  
  static VEC2_T   = 2 * sizeof.F32;
  static VEC3_T   = 3 * sizeof.F32;
  
  static LUMP_T   =  sizeof.U32 + sizeof.U32;
};

class lump_t {
  constructor(fileofs, filelen)
  {
    this.fileofs = fileofs;
    this.filelen = filelen;
  }
};

class mdl_file_lump {
  static VERTICES       = 0;
  static BSP_NODES      = 1;
  static MESH_GROUPS   = 2;
  static MATERIALS      = 3;
};

class mdl_file_mtl_t {
  constructor(u8_name)
  {
    this.u8_name = u8_name;
  }
};

class mdl_file_mesh_group_t {
  constructor(mtl_id, vertexofs, vertexlen)
  {
    this.mtl_id = mtl_id;
    this.vertexofs = vertexofs;
    this.vertexlen = vertexlen;
  }
};

export function mdl_file_save(mdl, path)
{
  const write = new write_t();
  
  const [vertices, mesh_groups] = flatten_objects(mdl.objects);
  const mtls = flatten_mtls(mdl.mtls);
  
  write.seek(3 * sizeof.LUMP_T);
  
  const vertices_fileofs = write.tell();
  for (const vertex of vertices)
    write.write_vertex(vertex);
  const vertices_filelen = write.tell() - vertices_fileofs;
  
  const mesh_group_fileofs = write.tell();
  for (const mesh_group of mesh_groups)
    write.write_mesh_group(mesh_group);
  const mesh_group_filelen = write.tell() - mesh_group_fileofs;
  
  const mtls_fileofs = write.tell();
  for (const mtl of mtls)
    write.write_mtl(mtl);
  const mtls_filelen = write.tell() - mtls_fileofs;
  
  const lump_vertices = new lump_t(vertices_fileofs, vertices_filelen);
  const lump_mesh_group = new lump_t(mesh_group_fileofs, mesh_group_filelen);
  const lump_mtls = new lump_t(mtls_fileofs, mtls_filelen);
  
  write.seek(0);
  write.write_lump(lump_vertices);
  write.write_lump(lump_mesh_group);
  write.write_lump(lump_mtls);
  
  fs.writeFileSync(path, Buffer.from(write.data()));
}

function flatten_mtls(mtls)
{
  const mdl_mtls = [];
  
  for (const mtl of mtls) {
    const u8_name = new Uint8Array(MAX_STRING_SIZE);
    
    for (let i = 0; i < mtl.name.length; i++)
      u8_name[i] = mtl.name[i].charCodeAt(0);
    
    mdl_mtls.push(new mdl_file_mtl_t(u8_name));
  }
  
  return mdl_mtls;
}

function flatten_objects(objects)
{
  const sorted_objects = objects.slice();
  
  const vertices = [];
  const mesh_groups = [];
  
  let vertexofs = 0;
  let objectofs = 0;
  let objectend = objectofs;
  
  while (objectend != sorted_objects.length) {
    vertices.push(...sorted_objects[objectend].vertices);
    
    const mtl_id = sorted_objects[objectend].mtl_id;
    
    for (let j = objectend + 1; j < sorted_objects.length; j++) {
      if (mtl_id == sorted_objects[j].mtl_id) {
        objectend++;
        
        const tmp = sorted_objects[objectend];
        sorted_objects[objectend] = sorted_objects[j];
        sorted_objects[j] = tmp;
        
        vertices.push(...sorted_objects[objectend].vertices);
      }
    }
    
    objectend++;
    
    const vertexlen = vertices.length - vertexofs;
    const mesh_group = new mdl_file_mesh_group_t(mtl_id, vertexofs, vertexlen);
    mesh_groups.push(mesh_group);
    
    vertexofs = vertices.length;
  }
  
  return [vertices, mesh_groups];
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
  
  write_mesh_group(mesh_group)
  {
    this.write_u32(mesh_group.mtl_id);
    this.write_u32(mesh_group.vertexofs);
    this.write_u32(mesh_group.vertexlen);
  }
  
  write_mtl(mtl)
  {
    for (let i = 0; i < MAX_STRING_SIZE; i++)
      this.write_u8(mtl.u8_name[i]);
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
