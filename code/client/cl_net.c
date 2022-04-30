#include "cl_local.h"

#include "input.h"
#include "../common/net.h"
#include "../game/protocol.h"
#include <string.h>

void cl_connect(const char *host)
{
  cl.sock = net_connect(host);
}

void cl_parse()
{
  frame_t frame;
  while (net_sock_read(cl.sock, &frame, sizeof(frame_t)) > 0) {
    switch (frame.netcmd) {
    case NETCMD_CLIENT_INFO:
      cl_parse_client_info(&frame);
      break;
    case NETCMD_SNAPSHOT:
      cl_parse_snapshot(&frame);
      break;
    case NETCMD_USERCMD:
      break;
    }
  }
}

void cl_parse_client_info(const frame_t *frame)
{
  cl.connected = true;
  cg_set_ent_player(&cl.cg, frame->data.client_info.entity);
}

void cl_parse_snapshot(const frame_t *frame)
{
  cl.cmd_tail = frame->data.snapshot.ack;
  cl.snapshot = frame->data.snapshot.d;
}

void cl_send_cmd()
{
  usercmd_t usercmd;
  
  if (cl.connected) {
    in_base_move(&usercmd);
    
    frame_t frame;
    frame.netcmd = NETCMD_USERCMD;
    frame.data.usercmd = usercmd;
    
    net_sock_send(cl.sock, &frame, sizeof(frame_t));
    
    cl.cmd_queue[(cl.cmd_head++) % MAX_CMD_QUEUE] = usercmd;
  }
}
