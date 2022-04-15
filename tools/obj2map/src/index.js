"use strict";

import path from "path";
import { obj_parse } from "./obj.js";
import { map_from_obj } from "./map.js";
import { map_file_save } from "./map-file.js";

function main()
{
  if (process.argv.length != 4) {
    console.log("usage:", path.parse(process.argv[1]).name, "[obj-file] [map-file]");
    process.exit(1);
  }
  
  const input_obj = process.argv[2];
  const output_map = process.argv[3];
  
  const untitled_obj = obj_parse(input_obj);
  const untitled_map = map_from_obj(untitled_obj);
  
  map_file_save(untitled_map, output_map);
}

main();
