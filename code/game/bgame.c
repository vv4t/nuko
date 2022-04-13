#include "bg-local.h"

void bgame_update(bgame_t *bgame)
{
  bgame_motion_gravity(bgame);
  bgame_player_look(bgame);
  bgame_player_move(bgame);
  bgame_motion_integrate(bgame);
  bgame_clip_capsule_bsp(bgame);
  bgame_motion_clip(bgame);
  bgame_player_test_grounded(bgame);
}
