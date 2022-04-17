
class socket_t {
  constructor(host)
  {
    this.ws = new WebSocket(host);
    this.ws.binaryType = "arraybuffer";
    this.ws.onopen = this.ws_open;
    this.ws.onclose = this.ws_close
    this.ws.onmessage = this.ws_message;
  }
  
  ws_open(e)
  {
    console.log("connection established");
  }
  
  ws_message(e)
  {
    console.log("message:", event.data);
  }
  
  ws_close(e)
  {
    console.log("connection closed");
  }
};

function read_heap_string(ptr)
{
  let str = "";
  
  let c;
  while ((c = Module.HEAP8[ptr++]) != 0)
    str += String.fromCharCode(c);
  
  return str;
}

Module.print = function(text) {
  console.log(text);
};

Module.canvas = document.getElementById("canvas");
Module.sockets = [];
