#include "bg_local.h"

void pm_accelerate(bg_motion_t *motion, vec3_t wish_dir, float accel, float wish_speed)
{
  float current_speed = vec3_dot(motion->velocity, wish_dir);
  float add_speed = wish_speed - current_speed;
  
  if (add_speed < 0)
    return;
  
  float accel_speed = accel * wish_speed * BG_TIMESTEP;
  
  if (accel_speed > wish_speed)
    accel_speed = add_speed;
  
  motion->velocity = vec3_add(motion->velocity, vec3_mulf(wish_dir, accel_speed));
}

void pm_air_accelerate(bg_motion_t *motion, vec3_t wish_dir, float wish_speed)
{
  float current_speed = vec3_dot(motion->velocity, wish_dir);
  float add_speed = wish_speed - current_speed;
  
  if (add_speed < 0)
    return;
  
  motion->velocity = vec3_add(motion->velocity, vec3_mulf(wish_dir, add_speed));
}

void pm_friction(bg_motion_t *motion)
{
  float speed = vec3_length(motion->velocity);
  
  if (speed != 0) {
    float drop = 1.0f - BG_FRICTION * BG_TIMESTEP;
    motion->velocity = vec3_mulf(motion->velocity, drop);
  }
}

void pm_free_look(bg_transform_t *transform, float yaw, float pitch)
{
  vec3_t axis_up = vec3_init(0.0f, 1.0f, 0.0f);
  vec3_t axis_right = vec3_init(1.0f, 0.0f, 0.0f);
  
  quat_t rot_yaw = quat_init_rotation(axis_up, yaw);
  
  vec3_t axis_pitch = vec3_rotate(axis_right, rot_yaw);
  quat_t rot_pitch = quat_init_rotation(axis_pitch, pitch);
  
  transform->rotation = quat_mul(rot_pitch, rot_yaw);
}

#define BG_PLAYER_CHECK (BGC_PMOVE | BGC_CLIP)
void bg_pm_check_pos(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_PLAYER_CHECK) != BG_PLAYER_CHECK)
      continue;
    
    bg->pmove[i].on_ground = false;
    
    for (int j = 0; j < bg->clip[i].num_planes; j++) {
      if (bg->clip[i].planes[j].normal.y > BG_MIN_INCLINE) {
        bg->pmove[i].on_ground = true;
        break;
      }
    }
  }
}

#define BG_PLAYER_MOVE (BGC_TRANSFORM | BGC_CLIENT | BGC_CLIP)
void bg_pm_walk_move(bgame_t *bg)
{
  vec3_t axis_up = vec3_init(0.0f, 1.0f, 0.0f);
  
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_PLAYER_MOVE) != BG_PLAYER_MOVE)
      continue;
    
    float cmd_right = bg->client[i].usercmd.right;
    float cmd_forward = bg->client[i].usercmd.forward;
    
    if (cmd_right || cmd_forward) {
      vec3_t cmd_dir = vec3_init(cmd_right, 0.0f, cmd_forward);
      vec3_t view_dir = vec3_rotate(cmd_dir, bg->transform[i].rotation);
      vec3_t wish_dir = vec3_normalize(vec3_init(view_dir.x, 0.0f, view_dir.z));
      
      if (bg->pmove[i].on_ground)
        pm_accelerate(&bg->motion[i], wish_dir, 5.5f, 7.0f);
      else
        pm_air_accelerate(&bg->motion[i], wish_dir, 2.0f);
    }
    
    if (bg->pmove[i].on_ground) {
      pm_friction(&bg->motion[i]);
      
      if (bg->client[i].usercmd.jump)
        bg->motion[i].velocity.y += 8.0f;
    }
  }
}

#define BG_PLAYER_LOOK (BGC_TRANSFORM | BGC_CLIENT)
void bg_pm_free_look(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_PLAYER_LOOK) != BG_PLAYER_LOOK)
      continue;
    
    float cmd_yaw = bg->client[i].usercmd.yaw;
    float cmd_pitch = bg->client[i].usercmd.pitch;
    
    pm_free_look(&bg->transform[i], cmd_yaw, cmd_pitch);
  }
}
