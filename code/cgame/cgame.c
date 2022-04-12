#include "cgame.h"

static void cg_init_bg(cgame_t *cg)
{
  cg->bg.edict = &cg->edict;
  cg->bg.bgc_transform = cg->bgc_transform;
  cg->bg.bgc_client = cg->bgc_client;
  cg->bg.bgc_capsule = cg->bgc_capsule;
  cg->bg.bgc_clip = cg->bgc_clip;
  cg->bg.bgc_motion = cg->bgc_motion;
  cg->bg.bgc_pmove = cg->bgc_pmove;
  
  cg->bg.bsp = NULL;
}

static void cg_init_player(cgame_t *cg)
{
  cg->entity_player = edict_add_entity(&cg->edict, CGES_PLAYER);
  
  cg->bgc_capsule[cg->entity_player].radius = 0.5f;
  cg->bgc_capsule[cg->entity_player].height = 1.0f;
  cg->bgc_transform[cg->entity_player].pos.y = 3;
}

void cg_init(cgame_t *cg)
{
  edict_init(&cg->edict);
  
  cg_init_bg(cg);
  cg_init_player(cg);
}

void cg_send_cmd(cgame_t *cg, const usercmd_t *usercmd)
{
  cg->bgc_client[cg->entity_player].usercmd = *usercmd;
}

void cg_new_map(cgame_t *cg, const map_t *map)
{ 
  cg->bg.bsp = map_load_bsp_nodes(map);
}

void cg_update(cgame_t *cg)
{
  bgs_motion_gravity(&cg->bg);
  bgs_player_look(&cg->bg);
  bgs_player_move(&cg->bg);
  bgs_motion_integrate(&cg->bg);
  bgs_clip_capsule_bsp(&cg->bg);
  bgs_motion_clip(&cg->bg);
  bgs_player_test_grounded(&cg->bg);
}
