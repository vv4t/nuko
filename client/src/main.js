"use strict";

import { client_t } from "./client.js";

const TICKRATE = 15;

function main()
{
  const client = new client_t();
  
  setInterval(function() {
    client.update();
  }, TICKRATE);
}

main();
