#include "sv_local.h"

#include <string.h>

void sv_client_init(sv_client_t *client, sock_t sock)
{
  memset(client, 0, sizeof(sv_client_t));
  
  client->sock = sock;
  client->connected = true;
}

void sv_client_parse_frame(sv_client_t *client, const frame_t *frame)
{
  switch (frame->netcmd) {
  case NETCMD_CLIENT_INFO:
    break;
  case NETCMD_SNAPSHOT:
    break;
  case NETCMD_USERCMD:
    sv_client_parse_usercmd(client, frame);
    break;
  }
}

void sv_client_parse_usercmd(sv_client_t *client, const frame_t *frame)
{
  client->cmd_queue[(client->cmd_head++) % MAX_CMD_QUEUE] = frame->data.usercmd;
}

const usercmd_t *sv_client_get_usercmd(sv_client_t *client)
{
  return &client->cmd_queue[(client->cmd_tail++) % MAX_CMD_QUEUE];
}

void sv_client_send_client_info(sv_client_t *client)
{
  frame_t frame;
  frame.netcmd = NETCMD_CLIENT_INFO;
  frame.data.client_info.entity = client->entity;
  
  net_sock_send(client->sock, &frame, sizeof(frame_t));
}
