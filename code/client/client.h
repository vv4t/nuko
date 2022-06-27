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
// NOTE: this snapshot differs from 'bg_snapshot_t' where the purpose of this
// structure is to hold all components to be interpolated.
typedef struct {
  bg_transform_t  transform[MAX_ENTITIES];
  bg_attack_t     attack[MAX_ENTITIES];
  bg_particle_t   particle[MAX_ENTITIES];
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
  entity_t        ent_client; // Local entity ID
} cgame_t;

extern cgame_t cg;

//
// cl_main.c
//

// Initialize the client
void cl_init();
void cl_init_local();

// Update the client
void cl_update(int delta_time);

#endif
