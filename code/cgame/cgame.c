#include "cgame.h"

static void cgame_init_player(cgame_t *cgame)
{
  cgame->player_entity = edict_add_entity(&cgame->edict, CGES_PLAYER);
  cgame->bgame.capsule[cgame->player_entity].radius = 0.5f;
  cgame->bgame.capsule[cgame->player_entity].height = 1.0f;
  cgame->bgame.transform[cgame->player_entity].position.y = 3;
  
  entity_t fumo_entity = edict_add_entity(&cgame->edict, CGES_FUMO);
  cgame->model[fumo_entity] = CGMDL_FUMO_CIRNO;
  cgame->fumo_entity = fumo_entity;
}

void cgame_init(cgame_t *cgame)
{
  edict_init(&cgame->edict);
  bgame_init(&cgame->bgame, &cgame->edict);
  
  cgame_init_player(cgame);
}

void cgame_send_cmd(cgame_t *cgame, const usercmd_t *usercmd)
{
  cgame->bgame.client[cgame->player_entity].usercmd = *usercmd;
}

void cgame_new_map(cgame_t *cgame, const map_t *map)
{
  bgame_new_map(&cgame->bgame, map);
  
  cgame->bgame.transform[cgame->player_entity].position = vec3_init(0.0f, 3.0f, 0.0f);
  cgame->bgame.motion[cgame->player_entity].velocity = vec3_init(0.0f, 0.0f, 0.0f);
}

float t = 0.0f;

void cgame_update(cgame_t *cgame)
{
  t += 0.015;
  bgame_update(&cgame->bgame);
  
  vec3_t up = vec3_init(0.0f, 1.0f, 0.0f);
  
  cgame->bgame.transform[cgame->fumo_entity].rotation = quat_init_rotation(up, t);
}
