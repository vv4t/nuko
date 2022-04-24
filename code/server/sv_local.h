#ifndef SV_LOCAL_H
#define SV_LOCAL_H

#include "server.h"
#include "../game/protocol.h"

void sv_cull(server_t *sv);
void sv_accept(server_t *sv);
void sv_poll(server_t *sv);
void sv_send_all_snapshot(server_t *sv);
void sv_send_open(server_t *sv, sv_client_t *client);
void sv_recv_usercmd(server_t *sv, sv_client_t *client, const frame_t *frame);

#endif
