"use strict";

import path from "path";
import { obj_parse } from "./obj.js";
import { mdl_from_obj } from "./mdl.js";
import { mdl_file_save } from "./mdl-file.js";

function main()
{
  if (process.argv.length != 4) {
    console.log("usage:", path.parse(process.argv[1]).name, "[obj-file] [mdl-file]");
    process.exit(1);
  }
  
  const input_obj = process.argv[2];
  const output_mdl = process.argv[3];
  
  const untitled_obj = obj_parse(input_obj);
  const untitled_mdl = mdl_from_obj(untitled_obj);
  
  mdl_file_save(untitled_mdl, output_mdl);
}

main();
