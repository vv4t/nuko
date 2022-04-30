#include "sv_local.h"

void sv_accept()
{
  sock_t sock;
  while ((sock = net_accept()) != -1) {
    sv_client_t *client = sv_new_client(sv);
    sv_client_init(client, sock);
    
    client->entity = sg_new_client(&sv.sg);
    
    sv_client_send_client_info(client);
  }
}

sv_client_t *sv_new_client()
{
  sv_client_t *client = NULL;
  
  for (int i = 0; i < sv.num_clients; i++) {
    if (!sv.clients[i].connected)
      return &sv.clients[i];
  }
  
  return &sv.clients[sv.num_clients++];
}

void sv_parse()
{
  for (int i = 0; i < sv.num_clients; i++) {
    sv_client_t *client = &sv.clients[i];
    
    int read;
    frame_t frame;
    
    while ((read = net_sock_read(client->sock, &frame, sizeof(frame_t))) > 0)
      sv_client_parse_frame(client, &frame);
    
    if (!read) {
      sg_remove_client(&sv.sg, sv.clients[i].entity);
      sv.clients[i].connected = false;
    }
  }
}

void sv_send_snapshot()
{
  frame_t frame;
  frame.netcmd = NETCMD_SNAPSHOT;
  
  sg_server_snapshot(&frame.data.snapshot.d, &sv.sg);
  
  for (int i = 0; i < sv.num_clients; i++) {
    sv_client_t *client = &sv.clients[i];
    
    frame.data.snapshot.ack = client->cmd_tail - 1;
    sg_client_snapshot(&frame.data.snapshot.d, client->entity, &sv.sg);
    net_sock_send(client->sock, &frame, sizeof(frame_t));
  }
}
