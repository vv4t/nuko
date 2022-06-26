/*
-- sv_local.h --

Local definitions for server
*/
#ifndef SV_LOCAL_H
#define SV_LOCAL_H

#include "server.h"

#include "../game/bgame.h"
#include "../game/frame.h"
#include "../game/map_file.h"
#include "../common/net.h"
#include "../common/log.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_DMG             8 // Maximum types of damage to be applied to the player

#define MAX_CMD_QUEUE       4
#define MAX_SNAPSHOT_QUEUE  64

#define SV_ROUND_TIME       180000
#define SV_RESTART_TIME     5000 // How long it takes to restart

// Server components
typedef enum {
  SVC_CLIENT          = (AUX_BGC << 0),
  SVC_RESPAWN         = (AUX_BGC << 1),
  SVC_SCORE           = (AUX_BGC << 2),
  SVC_DAMAGE          = (AUX_BGC << 3)
} sv_component_t;

// Server entity states
typedef enum {
  SV_ES_CLIENT        = SVC_CLIENT | BG_ES_CLIENT | SVC_SCORE | SVC_DAMAGE | SVC_RESPAWN,
  SV_ES_RESPAWN       = SVC_CLIENT | BGC_TRANSFORM | BGC_PARTICLE | SVC_RESPAWN | SVC_SCORE
} sv_entitystate_t;

// Originally I was envisioning a much larger scope of the game and imagined
// types of damage such as "bleeding" or "fall" damage which each could be
// queued. However, the only damage being shooting makes it somewhat excessive.
// However it still is used to identify who killed the player. Useful in cases
// where multiple shoot the same player in a single frame.
typedef struct {
  int         amount;
  entity_t    src;
} dmg_t;

typedef struct {
  sock_t    sock;
  
  int       snapshot_ack; // Last snapshot they acknowledged
  
  char      name[32];
  
  // Usercmd queue
  int       cmd_head;
  int       cmd_tail;
  usercmd_t cmd_queue[MAX_CMD_QUEUE];
} sv_client_t;

typedef struct {
  int       spawn_time;
  int       invul_time; // Invulnerable time
  bool      alive;
} sv_respawn_t;

typedef struct {
  // Damage queue
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
  
  // Round state
  int           round_count;
  int           round_time; // The remaining round time in milliseconds
  bool          round_start;
  int           num_clients; // Actual number of clients. NOTE: edict.num_entities counts deallocated entities within the space
  
  // Components
  sv_client_t   client[MAX_ENTITIES];
  sv_respawn_t  respawn[MAX_ENTITIES];
  sv_score_t    score[MAX_ENTITIES];
  sv_damage_t   damage[MAX_ENTITIES];
  
  // Snapshot queue
  int           snapshot_head;
  snapshot_t    snapshot_queue[MAX_SNAPSHOT_QUEUE];
} server_t;

extern server_t sv;
extern int      host_frametime;

//
// sv_game.c
//

bool      weapon_attack_pistol(
  vec3_t              weap_pos,
  vec3_t              weap_dir,
  vec3_t              victim_pos,
  const bg_capsule_t  *victim_capsule);

bool      weapon_attack_katana(
  vec3_t              weap_pos,
  vec3_t              weap_dir,
  vec3_t              victim_pos,
  const bg_capsule_t  *victim_capsule);

typedef bool (*weapon_attack_t)(vec3_t weap_pos, vec3_t weap_dir, vec3_t victim_pos, const bg_capsule_t *capsule);

static const weapon_attack_t weapon_attacks[] = {
  weapon_attack_pistol,
  weapon_attack_katana
};

// Queue entity to be applied to an entity
void      dmg_add(sv_damage_t *damage, int amount, entity_t src);

// General update function for sv_game
void      sv_game_update();

// Take a usercmd from it's queue and set it as the current usercmd for the
// frame
void      sv_client_move();

// Do shooting on the server side
void      sv_client_shoot();

// Take a general snapshot of the server
void      sv_server_snapshot(snapshot_t *snapshot);

// Take a snapshot of the server for a specific client
void      sv_client_snapshot(snapshot_t *snapshot, entity_t entity);

// Write the scoreboard into a string
bool      sv_print_score(char *dest, int len);

// Manages the round: countdown, restarting, etc. based on time
void      sv_round_status();

void      sv_load_map(const char *map);
void      sv_round_start();
void      sv_round_end();
void      sv_spawn_client(entity_t entity);
void      sv_apply_damage();
void      sv_check_respawn();

//
// sv_net.c
//

// Accept incoming clients
void      sv_accept();

// Parse client packets
void      sv_parse();

// Broadcasts to clients
void      sv_send_snapshot();
void      sv_send_client_info();
void      sv_send_chat(const char *text);

//
// sv_client.c
//

// Initialise new client
entity_t  sv_new_client(sock_t sock);

// Parse frames from the client
void      sv_client_parse_frame(entity_t entity, const frame_t *frame);
void      sv_client_parse_usercmd(entity_t entity, const frame_t *frame);
void      sv_client_parse_name(entity_t entity, const frame_t *frame);
void      sv_client_parse_chat(entity_t entity, const frame_t *frame);
void      sv_client_parse_score(entity_t entity, const frame_t *frame);

// Send frames to the client
void      sv_client_send_client_info(entity_t entity);
void      sv_client_send_chat(entity_t entity, const char *text);

// Disconnect the client from server
void      sv_client_disconnect(entity_t entity);

#endif
