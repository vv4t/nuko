#include "sv_local.h"

sv_client_t *sv_new_client(server_t *sv, int sock_id, entity_t entity)
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
  
  client->sock_id = sock_id;
  client->entity = entity;
  client->frame_tail = 0;
  client->frame_head = 0;
  client->connected = true;
  
  return client;
}

void sv_accept(server_t *sv)
{
  int sock;
  while ((sock = net_accept()) != -1) {
    entity_t entity = sg_add_client(&sv->sg);
    
    sv_client_t *client = sv_new_client(sv, sock, entity);
    sv_send_open(sv, client);
  }
}

void sv_send_all_snapshot(server_t *sv)
{
  frame_t frame = {0};
  frame.netcmd = NETCMD_SNAPSHOT;
  
  sg_build_snapshot(&frame.data.snapshot, &sv->sg);
  
  for (int i = 0; i < sv->num_clients; i++) {
    frame.outgoing_ack = sv->clients[i].incoming_seq;
    frame.data.snapshot.cl_pmove = sv->sg.bg.pmove[sv->clients[i].entity];
    frame.data.snapshot.cl_motion = sv->sg.bg.motion[sv->clients[i].entity];
    
    net_sock_send(sv->clients[i].sock_id, &frame, sizeof(frame_t));
  }
}

void sv_send_open(server_t *sv, sv_client_t *client)
{
  frame_t frame;
  frame.netcmd = NETCMD_OPEN;
  frame.data.client_entity = client->entity;
  
  net_sock_send(client->sock_id, &frame, sizeof(frame_t));
}

void sv_recv_usercmd(server_t *sv, sv_client_t *client, const frame_t *frame)
{
  client->frame_queue[(client->frame_head++) % MAX_FRAME_QUEUE] = *frame;
}

void sv_poll(server_t *sv)
{
  for (int i = 0; i < sv->num_clients; i++) {
    sv_client_t *client = &sv->clients[i];
    
    int bytes_read;
    frame_t frame;
    
    while ((bytes_read = net_sock_read(client->sock_id, &frame, sizeof(frame_t))) > 0) {
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
    
    if (!bytes_read) {
      sg_remove_client(&sv->sg, sv->clients[i].entity);
      sv->clients[i].connected = false;
    }
  }
}
