#ifndef SERVER_H
#define SERVER_H

#include "sgame.h"
#include "../game/protocol.h"
#include "../common/net.h"

#define MAX_CLIENTS 4
#define MAX_FRAME_QUEUE 128

typedef struct {
  int       sock_id;
  entity_t  entity;
  
  bool      connected;
  
  frame_t   frame_queue[MAX_FRAME_QUEUE];
  int       frame_head;
  int       frame_tail;
  
  int       incoming_seq;
} sv_client_t;

typedef struct {
  sgame_t     sg;
  
  sv_client_t clients[MAX_CLIENTS];
  int         num_clients;
} server_t;

void sv_init(server_t *sv);
void sv_update(server_t *sv);

#endif
