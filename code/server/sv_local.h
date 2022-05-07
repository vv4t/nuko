#ifndef SV_LOCAL_H
#define SV_LOCAL_H

#include "server.h"

#include "../game/bgame.h"
#include "../game/protocol.h"
#include "../game/map_file.h"
#include "../common/net.h"
#include "../common/log.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CMD_QUEUE       4
#define MAX_SNAPSHOT_QUEUE  64

typedef enum {
  SVC_CLIENT = (AUX_BGC << 0)
} sv_component_t;

typedef enum {
  SV_ES_CLIENT = (BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_MODEL | BGC_PMOVE) | SVC_CLIENT
} sv_entitystate_t;

typedef struct {
  sock_t    sock;
  
  int       snapshot_ack;
  
  char      name[32];
  
  int       cmd_head;
  int       cmd_tail;
  usercmd_t cmd_queue[MAX_CMD_QUEUE];
} sv_client_t;

typedef struct {
  bool      attack;
  int       next_attack;
} sv_client_t;

typedef struct {
  edict_t     edict;
  bgame_t     bg;
  
  const char  *map_name;
  
  sv_client_t client[MAX_ENTITIES];
  
  int         snapshot_head;
  snapshot_t  snapshot_queue[MAX_SNAPSHOT_QUEUE];
} server_t;

extern server_t sv;

//
// sv_game.c
//
bool      intersect_ray_capsule(
  vec3_t              origin,
  vec3_t              ray,
  vec3_t              offset,
  const bg_capsule_t  *capsule);

void      sv_game_update();
void      sv_client_move();
void      sv_client_shoot();
void      sv_server_snapshot(snapshot_t *snapshot);
void      sv_client_snapshot(snapshot_t *snapshot, entity_t entity);
void      sv_load_map(const char *map);

//
// sv_net.c
//
void      sv_accept();
void      sv_parse();
void      sv_send_snapshot();
void      sv_send_chat(const char *text);

//
// sv_client.c
//
entity_t  sv_new_client(sock_t sock);
void      sv_client_parse_frame(entity_t entity, const frame_t *frame);
void      sv_client_parse_usercmd(entity_t entity, const frame_t *frame);
void      sv_client_parse_name(entity_t entity, const frame_t *frame);
void      sv_client_parse_chat(entity_t entity, const frame_t *frame);
void      sv_client_send_client_info(entity_t entity);
void      sv_client_send_chat(entity_t entity, const char *text);

#endif
