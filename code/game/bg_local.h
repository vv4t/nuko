#include "bgame.h"

#define BG_MAX_INCLINE (45.0f * M_PI / 180.0f)
#define BG_TIMESTEP 0.015
#define BG_SENSITIVITY 0.005
#define BG_DRAG 0.9
#define BG_GRAVITY 18.0

void bg_player_move(bgame_t *bg);
void bg_player_look(bgame_t *bg);
void bg_clip_capsule_bsp(bgame_t *bg);
void bg_motion_gravity(bgame_t *bg);
void bg_motion_clip(bgame_t *bg);
void bg_motion_integrate(bgame_t *bg);
void bg_player_test_grounded(bgame_t *bg);
