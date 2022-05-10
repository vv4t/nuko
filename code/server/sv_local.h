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

#define MAX_DMG             8

#define MAX_CMD_QUEUE       4
#define MAX_SNAPSHOT_QUEUE  64

typedef enum {
  SVC_CLIENT          = (AUX_BGC << 0),
  SVC_ATTACK          = (AUX_BGC << 1),
  SVC_RESPAWN         = (AUX_BGC << 2),
  SVC_SCORE           = (AUX_BGC << 3),
  SVC_DAMAGE          = (AUX_BGC << 4)
} sv_component_t;

typedef enum {
  SV_ES_CLIENT        = SVC_CLIENT | BG_ES_CLIENT | SVC_ATTACK | SVC_SCORE | SVC_DAMAGE,
  SV_ES_RESPAWN       = SVC_CLIENT | BGC_TRANSFORM | SVC_RESPAWN | SVC_SCORE
} sv_entitystate_t;

typedef struct {
  int         amount;
  const char  *src;
} dmg_t;

typedef struct {
  sock_t    sock;
  
  int       snapshot_ack;
  
  char      name[32];
  
  int       cmd_head;
  int       cmd_tail;
  usercmd_t cmd_queue[MAX_CMD_QUEUE];
} sv_client_t;

typedef struct {
  bool      ready;
} sv_attack_t;

typedef struct {
  int       spawn_time;
} sv_respawn_t;

typedef struct {
  dmg_t       dmg[MAX_DMG];
  int         num_dmg;
} sv_damage_t;

typedef struct {
  int       kills;
  int       deaths;
} sv_score_t;

typedef struct {
  edict_t       edict;
  bgame_t       bg;
  
  const char    *map_name;
  
  sv_client_t   client[MAX_ENTITIES];
  sv_attack_t   attack[MAX_ENTITIES];
  sv_respawn_t  respawn[MAX_ENTITIES];
  sv_score_t    score[MAX_ENTITIES];
  sv_damage_t   damage[MAX_ENTITIES];
  
  int           snapshot_head;
  snapshot_t    snapshot_queue[MAX_SNAPSHOT_QUEUE];
} server_t;

extern server_t sv;
extern int      host_frametime;

//
// sv_game.c
//
bool      intersect_ray_capsule(
  vec3_t              origin,
  vec3_t              ray,
  vec3_t              offset,
  const bg_capsule_t  *capsule);

void      dmg_add(sv_damage_t *damage, int amount, const char *src);

void      sv_game_update();
void      sv_client_move();
void      sv_client_shoot();
void      sv_apply_damage();
void      sv_check_respawn();
void      sv_server_snapshot(snapshot_t *snapshot);
void      sv_client_snapshot(snapshot_t *snapshot, entity_t entity);
bool      sv_print_score(char *dest, int len);
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
void      sv_client_parse_score(entity_t entity, const frame_t *frame);
void      sv_client_send_client_info(entity_t entity);
void      sv_client_send_chat(entity_t entity, const char *text);

#endif
