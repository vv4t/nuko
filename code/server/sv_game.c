#include "sv_local.h"

void sv_game_update()
{
  if (!sv.num_clients) // Don't run the game if no one is playing
    return;
  
  sv_round_status();
  sv_check_respawn();
  sv_client_move();
  bg_update(&sv.bg);
  sv_client_shoot();
  sv_apply_damage();
}

void sv_round_start()
{
  // Map rotation set
  static const char *map_rotation[] = {
    "nk_neo",
    "nk_yuu",
    "nk_chito"
  };
  
  int map_num = sv.round_count++ % (sizeof(map_rotation) / sizeof(char*)); // Rotate the map per round
  sv_load_map(map_rotation[map_num]);
  
  memset(sv.score, 0, sizeof(sv.score)); // Clear scoreboard
  
  for (int i = 0; i < sv.edict.num_entities; i++) { // Clear dead clients
    if ((sv.edict.entities[i] & SVC_CLIENT) == SVC_CLIENT)
      sv_spawn_client(i);
    else
      edict_remove_entity(&sv.edict, i);
  }
  
  sv.round_time = SV_ROUND_TIME;
  sv.round_start = true;
}

void sv_round_status()
{
  // NOTE: this is ran every frame and should and should be evaluated as such.
  if (sv.round_start) {
    sv.round_time -= host_frametime;
    
    if (sv.round_time > 0) {
      if (sv.num_clients < 2) { // if the player count drops to less than 2, stop the game
        sv_send_chat("[SERVER] less than the required 2 players.");
        sv_round_end();
        sv.round_time = 0;
        sv.round_start = false;
      }
    } else {
      if (sv.round_time > -SV_RESTART_TIME) { // begin countdown until next round
        int remaining_secs = (SV_RESTART_TIME + sv.round_time) / 1000;
        
        if ((sv.round_time % 1000) == 0) { // Broadcast the remaining time every second
          if (remaining_secs == SV_RESTART_TIME / 1000)
            sv_round_end();
          
          static char msg[128];
          if (snprintf(msg, sizeof(msg), "[SERVER] restarting in %is.", remaining_secs) < sizeof(msg))
            sv_send_chat(msg);
        }
      } else {
        sv.round_start = false;
        // NOTE: the round is marked as inactive so that the next round doesn't
        // start unless the conditions are met (2 active players at least)
      }
    }
  } else if (sv.num_clients >= 2) { // Make sure there are at least two players
    sv_round_start();
  } else {
    sv.round_time = 0; // Not enough players, wait...
  }
}

void sv_round_end()
{
  sv_send_chat("[SERVER] round ended.");
  
  static char msg[256];
  if (sv_print_score(msg, sizeof(msg)))
    sv_send_chat(msg);
}

void sv_load_map(const char *name)
{
  sv.map_name = name;
  
  // Build map path string
  char map_path[256];
  sprintf(map_path, "assets/map/%s.map", name);
  
  // Load it into a map_t
  map_t map;
  
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "sv_load_map(): failed to load %s", map_path);
  
  bg_new_map(&sv.bg, &map); // Load the new map into the game
  sv_send_client_info(); // Broadcast a new map has been loaded
}

#define SV_CLIENT_SNAPSHOT (BGC_TRANSFORM | BGC_CLIENT | BGC_CAPSULE | BGC_CLIP | BGC_MOTION | BGC_PMOVE | BGC_HEALTH | BGC_ATTACK | BGC_WEAPON)
void sv_client_snapshot(snapshot_t *snapshot, entity_t entity)
{
  // Copy over the local client based components
  memcpy(&snapshot->cl_pmove, &sv.bg.pmove[entity], sizeof(bg_pmove_t));
  memcpy(&snapshot->cl_motion, &sv.bg.motion[entity], sizeof(bg_motion_t));
  memcpy(&snapshot->cl_health, &sv.bg.health[entity], sizeof(bg_health_t));
  
  snapshot->cl_entity_state = sv.edict.entities[entity] & SV_CLIENT_SNAPSHOT;
}

#define SV_SERVER_SNAPSHOT (BGC_TRANSFORM | BGC_MODEL | BGC_CAPSULE | BGC_ATTACK | BGC_WEAPON | BGC_PARTICLE)
void sv_server_snapshot(snapshot_t *snapshot)
{
  // Copy over global server based components
  memcpy(&snapshot->edict, &sv.edict, sizeof(edict_t));
  memcpy(&snapshot->sv_transform, &sv.bg.transform, sizeof(sv.bg.transform));
  memcpy(&snapshot->sv_capsule, &sv.bg.capsule, sizeof(sv.bg.capsule));
  memcpy(&snapshot->sv_model, &sv.bg.model, sizeof(sv.bg.model));
  memcpy(&snapshot->sv_attack, &sv.bg.attack, sizeof(sv.bg.attack));
  memcpy(&snapshot->sv_weapon, &sv.bg.weapon, sizeof(sv.bg.weapon));
  memcpy(&snapshot->sv_particle, &sv.bg.particle, sizeof(sv.bg.particle));
  
  // Include the roundtime
  snapshot->round_time = sv.round_time;
  
  // Apply bit mask so that only the shared components are visible
  for (int i = 0; i < snapshot->edict.num_entities; i++)
    snapshot->edict.entities[i] = sv.edict.entities[i] & SV_SERVER_SNAPSHOT;
}

#define SV_CLIENT_MOVE (BGC_CLIENT | SVC_CLIENT)
void sv_client_move()
{
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SV_CLIENT_MOVE) != SV_CLIENT_MOVE)
      continue;
    
    if (sv.client[i].cmd_tail < sv.client[i].cmd_head) {
      // Get the first usercmd from the client's queue
      int cmd_id = sv.client[i].cmd_tail++;
      usercmd_t *cmd = &sv.client[i].cmd_queue[cmd_id % MAX_CMD_QUEUE];
      
      // Use it
      sv.bg.client[i].usercmd = *cmd; 
    }
  }
}

#define SV_PRINT_SCORE (SVC_SCORE | SVC_CLIENT)
bool sv_print_score(char *dest, int dest_len)
{
  // Sort into entities based on number of kills
  entity_t ent_score[MAX_ENTITIES];
  int num_ent_score = 0;
  
  // Use an insertion sort to sort the score
  // This is advantageous as we do not "know" all the elements in the array
  // yet. Instead we insert elements as we sort the elements.
  
  // The first loop iterates over all entities
  for (int i = 0; i < sv.edict.num_entities; i++) {
    // As seen here, not all elements discovered in the first loop may need a
    // scoreboard entry
    if ((sv.edict.entities[i] & SV_PRINT_SCORE) != SV_PRINT_SCORE)
      continue;
    
    ent_score[num_ent_score++] = i;
    
    // The second loop iterates over all the discovered scoreboard entities
    // It is assumed to already to be sorted in descnding order from left to
    // right
    for (int j = num_ent_score - 2; j >= 0; j--) { // Start from the last element and loop backwards
      // Swap the entry until it is neither higher nor lower its neighbours
      // left to right respectively
      if (sv.score[ent_score[j]].kills < sv.score[ent_score[j + 1]].kills) {
        entity_t tmp = ent_score[j];
        ent_score[j] = ent_score[j + 1];
        ent_score[j + 1] = tmp;
      } else {
        break;
      }
    }
  }
  
  // Write the scoreboard into a string
  
  static char tmp[64]; // Holds a single entry temporarily
  int dest_ptr = 0; // String pointer
  
  for (int i = 0; i < num_ent_score; i++) {
    // Write a single entry into tmp
    int len = snprintf(tmp, sizeof(tmp) - 2, "%i. %-16s %i/%i\n",
      i + 1,
      sv.client[ent_score[i]].name,
      sv.score[ent_score[i]].kills,
      sv.score[ent_score[i]].deaths);
    
    // Write the null terminator if it's the last entry
    if (i == num_ent_score - 1)
      tmp[len - 1] = 0;
    
    if (dest_ptr + len > dest_len) { // Prevent buffer overflows
      log_printf(LOG_ERROR, "sv_print_score(): string too large");
      return false;
    }
    
    strcpy(&dest[dest_ptr], tmp);
    dest_ptr += len;
  }
  
  return true;
}

void sv_spawn_client(entity_t entity)
{
  int range = 20; // Square range which the player spawns in
  
  sv.edict.entities[entity] = SV_ES_CLIENT;
  
  // Spawn the player within a square of length 'range' relative to 0,0
  // NOTE: this does not do any collision checks!!!
  sv.bg.transform[entity].position.x = (rand() % range) - range / 2;
  sv.bg.transform[entity].position.y = 10;
  sv.bg.transform[entity].position.z = (rand() % range) - range / 2;
  
  sv.bg.motion[entity] = (bg_motion_t) {0};
  
  sv.bg.health[entity].now = sv.bg.health[entity].max;
  
  sv.respawn[entity].invul_time = 5000;
  sv.respawn[entity].alive = true;
}

#define SV_RESPAWN (SVC_RESPAWN)
void sv_check_respawn()
{
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SV_RESPAWN) != SV_RESPAWN)
      continue;
    
    if (sv.respawn[i].invul_time > 0)
      sv.respawn[i].invul_time -= host_frametime;
    
    if (!sv.respawn[i].alive) {
      if (sv.respawn[i].spawn_time > 0) {
        sv.respawn[i].spawn_time -= host_frametime;
        
        if ((sv.respawn[i].spawn_time % 1000) < host_frametime) {
          static char msg[256];
          snprintf(msg, 128, "[SERVER] respawning in %is", 1 + sv.respawn[i].spawn_time / 1000);
          sv_client_send_chat(i, msg);
        }
      } else {
        sv_spawn_client(i);
      }
    }
  }
}

#define SV_APPLY_DAMAGE (BGC_HEALTH | SVC_DAMAGE | SVC_RESPAWN)
void sv_apply_damage()
{
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SV_APPLY_DAMAGE) != SV_APPLY_DAMAGE)
      continue;
    
    if (sv.respawn[i].invul_time > 0) {
      sv.damage[i].num_dmg = 0;
      continue;
    }
    
    for (int j = 0; j < sv.damage[i].num_dmg; j++) {
      sv.bg.health[i].now -= sv.damage[i].dmg[j].amount;
      
      if (sv.bg.health[i].now <= 0) {
        sv.edict.entities[i] = SV_ES_RESPAWN;
        sv.respawn[i].spawn_time = 5000;
        sv.respawn[i].alive = false;
        
        sv.score[sv.damage[i].dmg[j].src].kills++;
        sv.score[i].deaths++;
        
        sv.bg.particle[i].now_time = 0;
        sv.bg.particle[i].end_time = 900;
        
        static char msg[128];
        if (snprintf(msg, sizeof(msg), "[SERVER] %s killed '%s'.", sv.client[sv.damage[i].dmg[j].src].name, sv.client[i].name) < sizeof(msg))
          sv_send_chat(msg);
        
        break;
      }
    }
    
    sv.damage[i].num_dmg = 0;
  }
}

#define SV_CLIENT_SHOOT (BGC_ATTACK | SVC_CLIENT)
#define SV_SHOOT_VICTIM (BGC_TRANSFORM | BGC_CAPSULE)
void sv_client_shoot()
{
  vec3_t forward = vec3_init(0.0f, 0.0f, 1.0f);
  
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SV_CLIENT_SHOOT) != SV_CLIENT_SHOOT)
      continue;
    
    if (!sv.bg.attack[i].active)
      continue;
    
    snapshot_t *snapshot = &sv.snapshot_queue[sv.client[i].snapshot_ack % MAX_SNAPSHOT_QUEUE];
    vec3_t weap_dir = vec3_rotate(forward, sv.bg.transform[i].rotation);
    
    for (int j = 0; j < snapshot->edict.num_entities; j++) {
      if ((snapshot->edict.entities[j] & SV_SHOOT_VICTIM) != SV_SHOOT_VICTIM)
        continue;
      
      if (i == j)
        continue;
      
      bool hit = weapon_attacks[sv.bg.weapon[i]](
        sv.bg.transform[i].position,
        weap_dir,
        snapshot->sv_transform[j].position,
        &snapshot->sv_capsule[j]);
      
      if (hit)
        dmg_add(&sv.damage[j], weapon_attribs[sv.bg.weapon[i]].damage, i);
    }
  }
}

void dmg_add(sv_damage_t *damage, int amount, entity_t src)
{
  dmg_t *dmg = &damage->dmg[damage->num_dmg];
  
  if (damage->num_dmg + 1 >= MAX_DMG) {
    log_printf(LOG_WARNING, "dmg_add(): too many dmg_t");
    return;
  }
  
  dmg->amount = amount;
  dmg->src = src;
  
  damage->num_dmg++;
}

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
  vec3_t weap_origin = vec3_add(weap_pos, weap_dir);
  vec3_t delta_pos = vec3_sub(weap_origin, victim_pos);
  float sphere_dist = 6 * victim_capsule->radius;
  
  if (vec3_dot(delta_pos, delta_pos) < sphere_dist * sphere_dist)
    return true;
  
  return false;
}
