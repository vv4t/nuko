"use strict";

import { obj_parse } from "./obj.js";
import { map_from_obj } from "./map.js";
import { write_map } from "./write-map.js";

function main()
{
  const untitled_obj = obj_parse("saves/nk_construct.obj");
  const untitled_map = map_from_obj(untitled_obj);
  
  write_map(untitled_map, "../../assets/map/untitled.map");
}

main();
