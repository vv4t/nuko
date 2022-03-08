"use strict";

import { vec3_t } from "../common/math.js";

export class motion_t {
  constructor()
  {
    this.prev_pos = new vec3_t();
    this.inverse_mass = 1.0;
    this.force = new vec3_t();
  }
};
