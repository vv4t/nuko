
function read_heap_string(ptr)
{
  let str = "";
  
  let c;
  while ((c = Module.HEAP8[ptr++]) != 0)
    str += String.fromCharCode(c);
  
  return str;
}

Module.print = console.log;
Module.canvas = document.getElementById("canvas");
