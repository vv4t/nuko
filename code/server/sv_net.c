#include "sv_local.h"

void sv_cull(server_t *sv)
{
  for (int i = 0; i < sv->num_clients; i++) {
    if (net_sock_status(sv->clients[i].sock) == NET_SOCK_DISCONNECTED) {
      sg_remove_client(&sv->sg, sv->clients[i].entity);
      sv->clients[i].connected = false;
    }
  }
}

sv_client_t *sv_new_client(server_t *sv, sock_t sock, entity_t entity)
{
  sv_client_t *client = NULL;
  
  for (int i = 0; i < sv->num_clients; i++) {
    if (!sv->clients[i].connected) {
      client = &sv->clients[i];
      break;
    }
  }
  
  if (!client)
    client = &sv->clients[sv->num_clients++];
  
  client->sock = sock;
  client->entity = entity;
  client->frame_tail = 0;
  client->frame_head = 0;
  client->connected = true;
  
  return client;
}

void sv_accept(server_t *sv)
{
  sock_t sock;
  while (net_accept(&sock)) {
    entity_t entity = sg_add_client(&sv->sg);
    
    sv_client_t *client = sv_new_client(sv, sock, entity);
    sv_send_open(sv, client);
  }
}

void sv_send_all_snapshot(server_t *sv)
{
  frame_t frame;
  frame.netcmd = NETCMD_SNAPSHOT;
  sg_build_snapshot(&frame.data.snapshot, &sv->sg);
  
  for (int i = 0; i < sv->num_clients; i++) {
    frame.outgoing_ack = sv->clients[i].incoming_seq;
    frame.data.snapshot.cl_pmove = sv->sg.bg.pmove[sv->clients[i].entity];
    frame.data.snapshot.cl_motion = sv->sg.bg.motion[sv->clients[i].entity];
    
    net_sock_send(sv->clients[i].sock, &frame, sizeof(frame_t));
  }
}

void sv_send_open(server_t *sv, sv_client_t *client)
{
  frame_t frame;
  frame.netcmd = NETCMD_OPEN;
  frame.data.client_entity = client->entity;
  
  net_sock_send(client->sock, &frame, sizeof(frame_t));
}

void sv_recv_usercmd(server_t *sv, sv_client_t *client, const frame_t *frame)
{
  client->frame_queue[(client->frame_head++) % MAX_FRAME_QUEUE] = *frame;
}

void sv_poll(server_t *sv)
{
  for (int i = 0; i < sv->num_clients; i++) {
    sv_client_t *client = &sv->clients[i];
    
    frame_t frame;
    while (net_sock_recv(client->sock, &frame, sizeof(frame_t))) {
      switch (frame.netcmd) {
      case NETCMD_OPEN:
        break;
      case NETCMD_SNAPSHOT:
        break;
      case NETCMD_USERCMD:
        sv_recv_usercmd(sv, client, &frame);
        break;
      }
    }
  }
}
