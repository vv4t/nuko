#include "sv_local.h"

void sv_accept()
{
  sock_t sock;
  while ((sock = net_accept()) != -1) { // check if there's a new incoming connection
    entity_t client = sv_new_client(sock); // create new entity for client
    if (client != -1) // accept if edict is free
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
    while ((read = frame_read(sv.client[i].sock, &frame)) > 0) // check for new frames
      sv_client_parse_frame(i, &frame);
    
    if (!read) // disconnect if there's nothing left to read i.e. client has disconnected
      sv_client_disconnect(i);
  }
}

void sv_send_snapshot()
{
  // take a snapshot of the server
  frame_t frame;
  frame.netcmd = NETCMD_SNAPSHOT;
  frame.data.snapshot.seq = sv.snapshot_head;
  sv_server_snapshot(&frame.data.snapshot.d);
  
  // add it to the cache
  sv.snapshot_queue[sv.snapshot_head++ % MAX_SNAPSHOT_QUEUE] = frame.data.snapshot.d; 
  
  // send the snapshot to all clients
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SVC_CLIENT) != SVC_CLIENT)
      continue;
    
    // snapshot of the individual client
    // NOTE: to prevent rewriting the snapshot with the same data, only half
    // the snapshot is written to. The other half is written by
    // 'sv_client_snapshot'
    frame.data.snapshot.ack = sv.client[i].cmd_tail - 1;
    sv_client_snapshot(&frame.data.snapshot.d, i);
    
    frame_send(sv.client[i].sock, &frame);
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
    
    frame_send(sv.client[i].sock, &frame);
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
