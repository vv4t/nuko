"use strict";

import { input_t } from "./input.js";
import { cgame_t } from "./cgame/cgame.js";
import { renderer_t } from "./renderer/renderer.js";

export class client_t {
  constructor()
  {
    this.input = new input_t();
    this.cgame = new cgame_t();
    this.renderer = new renderer_t(this.cgame);
  }
  
  update()
  {
    const usercmd = this.input.base_move();
    
    this.cgame.input(usercmd);
    this.cgame.update();
    this.renderer.render();
  }
};
