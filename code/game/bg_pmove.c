#include "bg_local.h"

// This is largely based off Quake's movement code, allowing for strafing and
// air control: https://adrianb.io/2015/02/14/bunnyhop.html
void pm_accelerate(bg_motion_t *motion, vec3_t wish_dir, float accel, float wish_speed)
{
  float current_speed = vec3_dot(motion->velocity, wish_dir); // How close is our current velocity to the wish direction?
  float add_speed = wish_speed - current_speed; // How much to add to achieve this velocity?
  
  // Don't reduce velocity
  if (add_speed < 0)
    return;
  
  // The speed to add per frame
  float accel_speed = accel * wish_speed * BG_TIMESTEP;
  
  // If the speed is over the speed to add, cap it
  if (accel_speed > wish_speed)
    accel_speed = add_speed;
  
  // Add the velocity
  motion->velocity = vec3_add(motion->velocity, vec3_mulf(wish_dir, accel_speed));
}

// The reason 'pm_accelerate' and 'pm_air_accelerate' are decoupled are because
// the walk code chooses to cap the speed only if it goes over. Whereas, this
// sets the speed directly to what is calculated. This produces movement closer
// to traditional Quake and Source-Engine strafe movements.
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
  // Friction is akin to a dampening value where v *= 0.99 is applied per frame
  // which slowly moves towards 0
  float drop = 1.0f - BG_FRICTION * BG_TIMESTEP;
  motion->velocity = vec3_mulf(motion->velocity, drop);
}

void pm_free_look(bg_transform_t *transform, float yaw, float pitch)
{
  vec3_t axis_up = vec3_init(0.0f, 1.0f, 0.0f);
  vec3_t axis_right = vec3_init(1.0f, 0.0f, 0.0f);
  
  // NOTE: this math is necessary to produce traditional FPS camera movement
  
  // Rotate around the 'y' axis how much the player is looking left to right
  quat_t rot_yaw = quat_init_rotation(axis_up, yaw);
  
  // Based on that, find the axis which to look up and down
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
    
    // Reset 'pmove' state
    bg->pmove[i].on_ground = false;
    
    // Check if the player is on the ground. This is done by checking all the
    // planes and checking if the player's alignment with the plane constitutes
    // as a slope 
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
    
    if (cmd_right || cmd_forward) { // Do we need to move?
      vec3_t cmd_dir = vec3_init(cmd_right, 0.0f, cmd_forward); // Non-aligned direction based on usercmd
      vec3_t view_dir = vec3_rotate(cmd_dir, bg->transform[i].rotation); // Rotated to player's view direction
      vec3_t wish_dir = vec3_normalize(vec3_init(view_dir.x, 0.0f, view_dir.z)); // Normalized so even if 'forward' or 'right' are weird values, the direction is all that matters
      
      if (bg->pmove[i].on_ground) // Change movement behaviour based on if the player is on the ground
        pm_accelerate(&bg->motion[i], wish_dir, 6.5f, 9.0f);
      else
        pm_air_accelerate(&bg->motion[i], wish_dir, 2.5f);
    }
    
    if (bg->pmove[i].on_ground) {
      pm_friction(&bg->motion[i]);
      
      if (bg->client[i].usercmd.jump)
        bg->motion[i].velocity.y += 8.0f;
      
      // NOTE: whereas in traditional source-based movement games, velocity is
      // preserved after jumping, because friction here is applied regardless
      // if they've jumped or not, the velocity is effectively capped. This was
      // a decision chosen as the game runs on a low tick rate which may lead
      // to incorrect collision if the velocity is too high.
      // This can be reversed by simply changing it to something like:
      // IF jump THEN ... ELSE apply_friction()
    }
  }
}

#define BG_PLAYER_LOOK (BGC_TRANSFORM | BGC_CLIENT)
void bg_pm_free_look(bgame_t *bg)
{
  // Mostly a wrapper for pm_free_look
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_PLAYER_LOOK) != BG_PLAYER_LOOK)
      continue;
    
    float cmd_yaw = bg->client[i].usercmd.yaw;
    float cmd_pitch = bg->client[i].usercmd.pitch;
    
    pm_free_look(&bg->transform[i], cmd_yaw, cmd_pitch);
  }
}

#define BG_PLAYER_ATTACK (BGC_TRANSFORM | BGC_CLIENT | BGC_ATTACK)
void bg_pm_attack(bgame_t *bg)
{
  for (int i = 0; i < bg->edict->num_entities; i++) {
    if ((bg->edict->entities[i] & BG_PLAYER_ATTACK) != BG_PLAYER_ATTACK)
      continue;
    
    if (bg->client[i].usercmd.attack && bg->attack[i].next_attack < 0) {
      bg->attack[i].active = true;
      bg->attack[i].next_attack = BG_ATTACK_TIME;
    } else {
      bg->attack[i].active = false;
      bg->attack[i].next_attack -= BG_TIMESTEP * 1000;
    }
  }
}
