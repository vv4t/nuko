#include "cgame.h"

static void cgame_init_player(cgame_t *cgame)
{
  cgame->entity_player = edict_add_entity(&cgame->edict, CGES_PLAYER);
  cgame->bgame.capsule[cgame->entity_player].radius = 0.5f;
  cgame->bgame.capsule[cgame->entity_player].height = 1.0f;
  cgame->bgame.transform[cgame->entity_player].position.y = 3;
}

void cgame_init(cgame_t *cgame)
{
  edict_init(&cgame->edict);
  bgame_init(&cgame->bgame, &cgame->edict);
  
  cgame_init_player(cgame);
}

void cgame_send_cmd(cgame_t *cgame, const usercmd_t *usercmd)
{
  cgame->bgame.client[cgame->entity_player].usercmd = *usercmd;
}

void cgame_new_map(cgame_t *cgame, const map_t *map)
{
  bgame_new_map(&cgame->bgame, map);
  
  cgame->bgame.transform[cgame->entity_player].position = vec3_init(0.0f, 3.0f, 0.0f);
  cgame->bgame.motion[cgame->entity_player].velocity = vec3_init(0.0f, 0.0f, 0.0f);
}

void cgame_update(cgame_t *cgame)
{
  bgame_update(&cgame->bgame);
}
