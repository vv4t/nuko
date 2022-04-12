#include "bgame.h"

#define BG_MAX_INCLINE (45.0f * M_PI / 180.0f)
#define BG_TIMESTEP 0.015
#define BG_SENSITIVITY 0.005
#define BG_DRAG 0.9

#include "../common/sys.h"

float player_accelerate(vec3_t vel, vec3_t wish_dir, float accel, float wish_speed)
{
  float current_speed = vec3_dot(vel, wish_dir);
  float add_speed = wish_speed - current_speed;
  
  if (add_speed < 0)
    return 0;
  
  if (accel > add_speed)
    return add_speed;
  
  return accel;
}

#define BGS_PLAYER_TEST_GROUNDED (BGC_PMOVE | BGC_CLIP)
void bgs_player_test_grounded(bgame_t *bg)
{
  vec3_t axis_up = vec3_init(0.0f, 1.0f, 0.0f);
  
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BGS_PLAYER_TEST_GROUNDED) != BGS_PLAYER_TEST_GROUNDED)
      continue;
    
    bg->bgc_pmove[i].is_grounded = false;
    for (int j = 0; j < bg->bgc_clip[i].num_planes; j++) {
      float cos_slope = vec3_dot(bg->bgc_clip[i].planes[j].normal, axis_up);
      bg->bgc_pmove[i].is_grounded = bg->bgc_pmove[i].is_grounded || (cos_slope > cos(BG_MAX_INCLINE));
    }
  }
}

#define BGS_CLIENT_MOVE (BGC_PMOVE | BGC_TRANSFORM | BGC_CLIENT)
void bgs_player_move(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BGS_CLIENT_MOVE) != BGS_CLIENT_MOVE)
      continue;
    
    float d_right = bg->bgc_client[i].usercmd.right;
    float d_forward = bg->bgc_client[i].usercmd.forward;
    
    if (d_right || d_forward) {
      vec3_t cmd_dir = vec3_init(d_right, 0.0f, d_forward);
      vec3_t wish_dir = vec3_rotate(cmd_dir, bg->bgc_pmove[i].move_rot);
      
      float add_speed;
      if (bg->bgc_pmove[i].is_grounded)
        add_speed = player_accelerate(bg->bgc_motion[i].vel, wish_dir, 1.0f, 6.0f);
      else
        add_speed = player_accelerate(bg->bgc_motion[i].vel, wish_dir, 4.0f, 1.5f);
      
      vec3_t add_vel = vec3_mulf(wish_dir, add_speed);
      
      bg->bgc_motion[i].vel = vec3_add(bg->bgc_motion[i].vel, add_vel);
    }
    
    if (bg->bgc_pmove[i].is_grounded) {
      bg->bgc_motion[i].vel.x *= BG_DRAG;
      bg->bgc_motion[i].vel.y *= BG_DRAG;
      bg->bgc_motion[i].vel.z *= BG_DRAG;
      
      if (bg->bgc_client[i].usercmd.jump)
        bg->bgc_motion[i].vel.y += 8.0f;
    }
  }
}

#define BGS_CLIENT_LOOK (BGC_PMOVE | BGC_TRANSFORM | BGC_CLIENT)
void bgs_player_look(bgame_t *bg)
{
  vec3_t axis_up = vec3_init(0.0f, 1.0f, 0.0f);
  vec3_t axis_right = vec3_init(1.0f, 0.0f, 0.0f);
  
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BGS_CLIENT_LOOK) != BGS_CLIENT_LOOK)
      continue;
    
    float d_yaw = bg->bgc_client[i].usercmd.yaw * BG_SENSITIVITY;
    float d_pitch = bg->bgc_client[i].usercmd.pitch * BG_SENSITIVITY;
    
    quat_t rot_yaw = quat_init_rotation(axis_up, d_yaw);
    
    vec3_t axis_pitch = vec3_rotate(axis_right, rot_yaw);
    quat_t rot_pitch = quat_init_rotation(axis_pitch, d_pitch);
    
    bg->bgc_pmove[i].move_rot = rot_yaw;
    bg->bgc_transform[i].rot = quat_mul(rot_pitch, rot_yaw);
  }
}
