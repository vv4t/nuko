#include "weapon.h"

bool weapon_attack_pistol(
  vec3_t              weap_pos,
  vec3_t              weap_dir,
  vec3_t              victim_pos,
  const bg_capsule_t  *victim_capsule)
{
  vec3_t delta_pos = vec3_sub(victim_pos, weap_pos);
  vec3_t delta_dir = vec3_normalize(delta_pos);
  
  float proj_dist = vec3_dot(delta_dir, weap_dir);
  
  if (proj_dist > 0) {
    vec3_t normal = vec3_normalize(vec3_add(delta_dir, vec3_mulf(weap_dir, -proj_dist)));
    float distance = vec3_dot(weap_pos, normal);
    
    float sphere_dist = vec3_dot(normal, victim_pos) - distance - 3 * victim_capsule->radius;
    
    if (sphere_dist < 0.0f)
      return true;
  }
  
  return false;
}

bool weapon_attack_katana(
  vec3_t              weap_pos,
  vec3_t              weap_dir,
  vec3_t              victim_pos,
  const bg_capsule_t  *victim_capsule)
{
  vec3_t weap_origin = vec3_add(weap_pos, vec3_mulf(weap_dir, 0.25));
  vec3_t delta_pos = vec3_sub(weap_origin, victim_pos);
  float sphere_dist = 6 * victim_capsule->radius;
  
  if (vec3_dot(delta_pos, delta_pos) < sphere_dist * sphere_dist)
    return true;
  
  return false;
}
