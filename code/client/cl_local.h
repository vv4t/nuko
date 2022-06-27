/*
-- cl_local.h --

The local definitions of the client module.
*/

#ifndef CL_LOCAL_H
#define CL_LOCAL_H

#include "client.h"

#include "input.h"
#include "renderer.h"
#include "../game/weapon.h"
#include "../game/frame.h"
#include "../game/map_file.h"
#include "../common/net.h"
#include "../common/cmd.h"
#include "../common/log.h"
#include "../common/sys.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define MAX_CMD_QUEUE   128

typedef enum {
  CONN_DISCONNECTED,
  CONN_CONNECTING,
  CONN_CONNECTED
} conn_status_t;

// Client components
typedef enum {
  CLC_DUMMY = (AUX_BGC << 0),
} cl_component_t;

typedef struct {
  // Network socket for TCP/WebSocket connection
  sock_t        sock;
  conn_status_t connection;
  
  // The last   snapshot received from the server
  int           snapshot_ack;
  snapshot_t    snapshot;
  
  // The client's usercmd set by polling peripheral input
  usercmd_t     usercmd;
  
  // A cache of unacknowledged usercmds used in cl_predict()
  int           cmd_tail;
  int           cmd_head;
  usercmd_t     cmd_queue[MAX_CMD_QUEUE];
} client_t;

extern client_t cl;

//
// - cl_main.c -
// Main functions called by other modules
//
void  cl_console();

//
// - cl_cgame.c -
// Synchronise with the server and produce a smooth game, responsive game state
// for the renderer
//
void  cl_snapshot();

// - Interpolate between two snapshots to produce a smooth inbetween state -
// This is necessary as the game will run at a lower update rate than frame
// rate. To produce produce smooth movement, two states of the game are
// interpolated between.
void  cl_interpolate(float interp);

// - Orient the camera based on the current usercmd -
// While this is also done through game logic, rotation is difficult to
// interpolate between and is therefore not considered in interpolation.
// Instead, the player's rotation is updated every frame. This is neceesary the
// camera's orientation is the most salient change between states.
void  cl_view_look();

// - "Predict" the future state of the game -
// Because of network latency, the game state received by the client will be
// behind by some amount. For the game to feel responsive the client simulates
// usercmds unacknolwedged by the server to produce a game state ahead of what
// is received.
void  cl_predict();

// - Copy over the last received game state -
void  cl_reconcile();

void  cl_load_map(const char *path);

//
// - cl_net.c -
// Maintain a network connection with the server/host. This involves parsing
// packets sent by the server and sending packets created by the client, 
//
void  cl_connect();
void  cl_parse();
void  cl_parse_snapshot(const frame_t *frame);
void  cl_parse_client_info(const frame_t *frame);
void  cl_parse_chat(const frame_t *frame);
void  cl_send_cmd();
void  cl_send_chat(const char *text);
void  cl_send_name(const char *name);
void  cl_send_score();

//
// - cl_tutorial.c -
// Default tutorial map before the player connects to the server
//
void  cl_tutorial_init();
void  cl_tutorial_spawn_dummy();
void  cl_tutorial_spawn_player();
void  cl_tutorial_attack();
void  cl_tutorial_dummy_death();
void  cl_tutorial_round();

#endif
