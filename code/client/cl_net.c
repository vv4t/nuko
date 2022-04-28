#include "client.h"

#include "../common/net.h"
#include "../game/protocol.h"
#include <string.h>

#ifdef __EMSCRIPTEN__

void cl_get_host_address(char *host_address, int len);

#else

void cl_get_host_address(char *host_address, int len)
{
  const char *str_host = "127.0.0.1";
  memcpy(host_address, str_host, strlen(str_host) + 1);
}

#endif

void cl_net_init(client_t *cl)
{
  char host_address[256];
  cl_get_host_address(host_address, 256);
  
  cl->connected = false;
  cl->sock_id = net_connect(host_address);
}

void cl_recv_open(client_t *cl, const frame_t *frame)
{
  cl->connected = true;
  cg_set_player(&cl->cg, frame->data.client_entity);
}

void cl_recv_snapshot(client_t *cl, const frame_t *frame)
{
  cl->snapshot = frame->data.snapshot;
  cl->incoming_ack = frame->outgoing_ack;
}

void cl_net_recv(client_t *cl)
{
  frame_t frame;
  while (net_sock_read(cl->sock_id, &frame, sizeof(frame_t)) > 0) {
    switch (frame.netcmd) {
    case NETCMD_OPEN:
      cl_recv_open(cl, &frame);
      break;
    case NETCMD_SNAPSHOT:
      cl_recv_snapshot(cl, &frame);
      break;
    case NETCMD_USERCMD:
      break;
    }
  }
}

void cl_send_cmd(client_t *cl)
{
  if (cl->connected) {
    frame_t frame;
    frame.netcmd = NETCMD_USERCMD;
    frame.data.usercmd = cl->usercmd;
    frame.outgoing_seq = cl->outgoing_seq;
    
    net_sock_send(cl->sock_id, &frame, sizeof(frame_t));
    
    cl->cmd_queue[frame.outgoing_seq % MAX_USERCMDS] = cl->usercmd;
    cl->outgoing_seq++;
  }
}
