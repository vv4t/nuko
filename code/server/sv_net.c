#include "sv_local.h"

void sv_accept(server_t *sv)
{
  sv_client_t *client = &sv->clients[sv->num_clients];
  
  while (net_accept(&client->sock)) {
    client->entity = sg_add_client(&sv->sg);
    client->frame_tail = 0;
    client->frame_head = 0;
    sv->num_clients++;
    
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
