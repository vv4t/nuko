/*
-- client.h (cl) --

The public definitions of the client module.

The client module is the main module of the client program. The client unifies
input, rendering and connecting and a synchronised game state between the host
server, creating a live, responsive and visual game.
*/

#ifndef CLIENT_H
#define CLIENT_H

#include "../game/edict.h"
#include "../game/bgame.h"

// A snapshot with components relevant to interpolation
typedef struct {
  bg_transform_t  transform[MAX_ENTITIES];
  bg_attack_t     attack[MAX_ENTITIES];
} cl_snapshot_t;

// The public state of the game used by the renderer
typedef struct {
  // Common local game state to be synchronised with server
  edict_t         edict;
  bgame_t         bg;
  
  // Snapshots of the game which are to be interpolated by cl_interpolate()
  cl_snapshot_t   to;
  cl_snapshot_t   from;
  cl_snapshot_t   tween;
  
  // Server round time
  int             round_time;
  
  // Local entity ID
  entity_t        ent_client;
} cgame_t;

extern cgame_t cg;

//
// cl_main.c
//

// Initialize the client
void cl_init();

// Update the client
void cl_update(
  int delta_time // The amount of time in milliseconds since the last update
);

#endif
