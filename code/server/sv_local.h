#ifndef SV_LOCAL_H
#define SV_LOCAL_H

#include "sgame.h"
#include "../game/protocol.h"
#include "../common/net.h"

#define MAX_CLIENTS   4
#define MAX_CMD_QUEUE 128

typedef struct {
  sock_t    sock;
  entity_t  entity;
  
  bool      connected;
  
  int       cmd_head;
  int       cmd_tail;
  usercmd_t cmd_queue[MAX_CMD_QUEUE];
} sv_client_t;

typedef struct {
  sgame_t     sg;
  
  sv_client_t clients[MAX_CLIENTS];
  int         num_clients;
} server_t;

//
// sv_main.c
//
void            sv_fixed_update();
void            sv_game_update();
void            sv_load_map(const char *map);

//
// sv_net.c
//
void            sv_accept();
void            sv_parse();
void            sv_send_snapshot();
sv_client_t     *sv_new_client();

//
// sv_client.c
//
void            sv_client_init(sv_client_t *client, sock_t sock);
void            sv_client_parse_frame(sv_client_t *client, const frame_t *frame);
void            sv_client_parse_usercmd(sv_client_t *client, const frame_t *frame);
void            sv_client_send_client_info(sv_client_t *client);
const usercmd_t *sv_client_get_usercmd(sv_client_t *client);

extern server_t sv;

#endif
