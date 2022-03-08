"use strict";

import { transform_t } from "./transform.js";
import { motion_t } from "./motion.js";

import { vec3_t, quat_t } from "../common/math.js";

export class cgame_t {
  constructor()
  {
    this.num_entities = 0;
    
    this.c_transform = {};
    this.c_motion = {};
    
    this.player = this.add_entity();
    this.c_transform[this.player] = new transform_t();
    this.c_motion[this.player] = new motion_t();
    
    this.c_transform[this.player].pos.z = -5;
    
    this.usercmd = null;
  }
  
  input(usercmd)
  {
    this.usercmd = usercmd;
  }
  
  update()
  {
    this.player_look();
    this.player_move();
  }
  
  integrate_motion()
  {
    for (let i = 0; i < this.num_entities; i++) {
      if (!this.c_transform[i] || !this.c_motion[i])
        continue;
      
      const pos = this.c_transform[i].pos;
      const old_pos = this.c_motion[i].old_pos;
      
      const vel = pos.sub(old_pos);
      
      const accel = this.c_motion[i].forces.mulf(this.c_motion[i].inverse_mass);
      
      const new_pos = pos.add(vel).add(accel.mulf(this.timestep_squared));
      
      this.c_motion[i].old_pos = pos;
      this.c_transform[i].pos = new_pos;
    }
  }
  
  player_look()
  {
    const SENSITIVITY = 0.001;
    
    const up = new vec3_t(0, 1, 0);
    const right = new vec3_t(1, 0, 0);
    
    const rot_yaw = quat_t.init_rotation(up, this.usercmd.yaw * SENSITIVITY);
    
    const axis_pitch = right.rotate(rot_yaw);
    const rot_pitch = quat_t.init_rotation(axis_pitch, this.usercmd.pitch * SENSITIVITY);
    
    this.c_transform[this.player].rot = rot_pitch.mul(rot_yaw);
  }
  
  player_move()
  {
    const cmd_dir = new vec3_t(this.usercmd.right, 0, this.usercmd.forward);
    const wish_dir = cmd_dir.rotate(this.c_transform[this.player].rot);
    
    this.c_transform[this.player].pos = this.c_transform[this.player].pos.add(wish_dir.mulf(0.1));
  }
  
  add_entity()
  {
    return this.num_entities++;
  }
}
