const main_in = document.getElementById("stdin");
const main_out = document.getElementById("stdout");
const main_canvas = document.getElementById("canvas");

let main_in_buf = "";

function main_print(text)
{
  main_out.value += text + "\n";
  main_out.scrollTop = stdout.scrollHeight;
}

function main_set_status(text)
{
  main_print(text);
}

main_in.onkeypress = function(e)
{
  if (e.keyCode == 13) {
    main_in_buf += main_in.value;
    main_in.value = "";
    return false;
  }
  
  return true;
};

function read_heap_string(ptr)
{
  let str = "";
  
  let c;
  while ((c = Module.HEAP8[ptr++]) != 0)
    str += String.fromCharCode(c);
  
  return str;
}

function write_heap_string(ptr, str, len)
{
  str_len = Math.min(len, str.length);
  
  for (let i = 0; i < str_len; i++)
    Module.HEAP8[ptr + i] = str.charCodeAt(i);
  
  Module.HEAP8[ptr + str_len] = 0;
  
  return str_len;
}

var Module = {
  preRun: [],
  postRun: [],
  print: main_print,
  setStatus: main_set_status,
  canvas: main_canvas
};
