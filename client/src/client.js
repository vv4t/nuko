"use strict";

import { renderer_t } from "./renderer/renderer.js";

export class client_t {
  constructor()
  {
    this.renderer = new renderer_t();
  }
  
  update()
  {
    this.renderer.render();
  }
};
