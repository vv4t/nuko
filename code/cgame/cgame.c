#include "cgame.h"

static void cgame_init_bgame(cgame_t *cgame)
{
  cgame->bgame.edict = &cgame->edict;
  cgame->bgame.transform = cgame->bg_transform;
  cgame->bgame.client = cgame->bg_client;
  cgame->bgame.capsule = cgame->bg_capsule;
  cgame->bgame.clip = cgame->bg_clip;
  cgame->bgame.motion = cgame->bg_motion;
  cgame->bgame.pmove = cgame->bg_pmove;
  
  cgame->bgame.bsp = NULL;
}

static void cgame_init_player(cgame_t *cgame)
{
  cgame->entity_player = edict_add_entity(&cgame->edict, CGES_PLAYER);
  cgame->bg_capsule[cgame->entity_player].radius = 0.5f;
  cgame->bg_capsule[cgame->entity_player].height = 1.0f;
  cgame->bg_transform[cgame->entity_player].position.y = 3;
}

void cgame_init(cgame_t *cgame)
{
  edict_init(&cgame->edict);
  
  cgame_init_bgame(cgame);
  cgame_init_player(cgame);
}

void cgame_send_cmd(cgame_t *cgame, const usercmd_t *usercmd)
{
  cgame->bg_client[cgame->entity_player].usercmd = *usercmd;
}

void cgame_new_map(cgame_t *cgame, const map_t *map)
{
  if (cgame->bgame.bsp)
    map_free_bsp(cgame->bgame.bsp);
  
  cgame->bgame.bsp = map_load_bsp(map);
}

void cgame_update(cgame_t *cgame)
{
  bgame_update(&cgame->bgame);
}
