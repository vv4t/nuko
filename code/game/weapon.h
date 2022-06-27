/*
-- weapon.h --

Shared functions for weapon attack patterns

*/

#include "bgame.h"

bool      weapon_attack_pistol(
  vec3_t              weap_pos,
  vec3_t              weap_dir,
  vec3_t              victim_pos,
  const bg_capsule_t  *victim_capsule);

bool      weapon_attack_katana(
  vec3_t              weap_pos,
  vec3_t              weap_dir,
  vec3_t              victim_pos,
  const bg_capsule_t  *victim_capsule);

typedef bool (*weapon_attack_t)(vec3_t weap_pos, vec3_t weap_dir, vec3_t victim_pos, const bg_capsule_t *capsule);

static const weapon_attack_t weapon_attacks[] = {
  weapon_attack_pistol,
  weapon_attack_katana
};

