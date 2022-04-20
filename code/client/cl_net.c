#include "client.h"

#include "../common/net.h"
#include "../game/protocol.h"

extern void cl_get_host_address(char *host_address, int max);

void cl_net_init(client_t *client)
{
  char host_address[256];
  cl_get_host_address(host_address, 256);
  
  client->sock = net_connect(host_address);
}

void cl_recv_open(client_t *client, const frame_t *frame)
{
  cg_set_player(&client->cg, frame->data.client_entity);
}

void cl_recv_snapshot(client_t *client, const frame_t *frame)
{
  client->cg.snapshot = frame->data.snapshot;
  client->cg.incoming_ack = frame->outgoing_ack;
}

void cl_poll(client_t *client)
{
  frame_t frame;
  while (net_sock_recv(client->sock, &frame, sizeof(frame_t))) {
    switch (frame.netcmd) {
    case NETCMD_OPEN:
      cl_recv_open(client, &frame);
      break;
    case NETCMD_SNAPSHOT:
      cl_recv_snapshot(client, &frame);
      break;
    case NETCMD_USERCMD:
      break;
    }
  }
}

void cl_send_cmd(client_t *client)
{
  frame_t frame;
  frame.netcmd = NETCMD_USERCMD;
  frame.outgoing_seq = client->cg.outgoing_seq++;
  frame.data.usercmd = client->usercmd;
  
  net_sock_send(client->sock, &frame, sizeof(frame_t));
}
