#include "sv_local.h"

void sv_accept()
{
  sock_t sock;
  while ((sock = net_accept()) != -1) {
    entity_t client = sv_new_client(sock);
    if (client != -1)
      sv_client_send_client_info(client);
  }
}

void sv_parse()
{
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SVC_CLIENT) != SVC_CLIENT)
      continue;
    
    int read;
    
    frame_t frame;
    while ((read = net_sock_read(sv.client[i].sock, &frame, sizeof(frame_t))) > 0)
      sv_client_parse_frame(i, &frame);
    
    if (!read)
      sv_client_disconnect(i);
  }
}

void sv_send_snapshot()
{
  frame_t frame;
  frame.netcmd = NETCMD_SNAPSHOT;
  frame.data.snapshot.seq = sv.snapshot_head;
  sv_server_snapshot(&frame.data.snapshot.d);
  
  sv.snapshot_queue[sv.snapshot_head++ % MAX_SNAPSHOT_QUEUE] = frame.data.snapshot.d; 
  
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SVC_CLIENT) != SVC_CLIENT)
      continue;
    
    frame.data.snapshot.ack = sv.client[i].cmd_tail - 1;
    sv_client_snapshot(&frame.data.snapshot.d, i);
    
    net_sock_send(sv.client[i].sock, &frame, sizeof(frame_t));
  }
}

void sv_send_chat(const char *text)
{
  frame_t frame;
  frame.netcmd = NETCMD_CHAT;
  strncpy(frame.data.chat.content, text, sizeof(frame.data.chat.content));
  
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SVC_CLIENT) != SVC_CLIENT)
      continue;
    
    net_sock_send(sv.client[i].sock, &frame, sizeof(frame_t));
  }
}

void sv_send_client_info()
{
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SVC_CLIENT) != SVC_CLIENT)
      continue;
    
    sv_client_send_client_info(i);
  }
}
