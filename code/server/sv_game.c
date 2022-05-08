#include "sv_local.h"

void sv_game_update()
{
  sv_respawn();
  sv_client_move();
  bg_update(&sv.bg);
  sv_client_shoot();
}

void sv_load_map(const char *name)
{
  sv.map_name = name;
  
  char map_path[256];
  sprintf(map_path, "assets/map/%s.map", name);
  
  map_t map;
  
  if (!map_load(&map, map_path))
    log_printf(LOG_FATAL, "cl_load_map(): failed to load %s", map_path);
  
  bg_new_map(&sv.bg, &map);
}

void sv_client_snapshot(snapshot_t *snapshot, entity_t entity)
{
  memcpy(&snapshot->cl_pmove, &sv.bg.pmove[entity], sizeof(bg_pmove_t));
  memcpy(&snapshot->cl_motion, &sv.bg.motion[entity], sizeof(bg_motion_t));
  
  snapshot->cl_entity = sv.edict.entities[entity] & BG_ES_CLIENT;
}

#define SV_SERVER_SNAPSHOT (BGC_TRANSFORM | BGC_CAPSULE | BGC_MODEL)
void sv_server_snapshot(snapshot_t *snapshot)
{
  memcpy(&snapshot->edict, &sv.edict, sizeof(edict_t));
  memcpy(&snapshot->sv_transform, &sv.bg.transform, sizeof(sv.bg.transform));
  memcpy(&snapshot->sv_capsule, &sv.bg.capsule, sizeof(sv.bg.capsule));
  
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
      int cmd_id = sv.client[i].cmd_tail++;
      usercmd_t *cmd = &sv.client[i].cmd_queue[cmd_id % MAX_CMD_QUEUE];
      sv.bg.client[i].usercmd = *cmd; 
    }
  }
}

#define SV_PRINT_SCORE (SVC_SCORE | SVC_CLIENT)
bool sv_print_score(char *dest, int dest_len)
{
  entity_t ent_score[MAX_ENTITIES];
  int num_ent_score = 0;
  
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SV_PRINT_SCORE) != SV_PRINT_SCORE)
      continue;
    
    ent_score[num_ent_score++] = i;
    
    for (int j = num_ent_score - 2; j >= 0; j--) {
      if (sv.score[ent_score[j]].kills < sv.score[ent_score[j + 1]].kills) {
        entity_t tmp = ent_score[j];
        ent_score[j] = ent_score[j + 1];
        ent_score[j + 1] = tmp;
      } else {
        break;
      }
    }
  }
  
    
  static char tmp[64];
  int dest_ptr = 0;
  
  for (int i = 0; i < num_ent_score; i++) {
    int len = snprintf(tmp, sizeof(tmp) - 2, "%i. %-16s %i/%i\n",
      i + 1,
      sv.client[ent_score[i]].name,
      sv.score[ent_score[i]].kills,
      sv.score[ent_score[i]].deaths);
    
    if (i == num_ent_score - 1)
      tmp[len - 1] = 0;
    
    if (dest_ptr + len > dest_len) {
      log_printf(LOG_ERROR, "sv_print_score(): string too large");
      return false;
    }
    
    strcpy(&dest[dest_ptr], tmp);
    dest_ptr += len;
  }
  
  return true;
}

#define SV_RESPAWN (SVC_RESPAWN)
void sv_respawn()
{
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SV_RESPAWN) != SV_RESPAWN)
      continue;
    
    if (sv.respawn[i].spawn_time > 0) {
      sv.respawn[i].spawn_time -= host_frametime;
      
      if ((sv.respawn[i].spawn_time % 1000) < host_frametime) {
        static char msg[256];
        snprintf(msg, 128, "[SERVER] respawning in %is", 1 + sv.respawn[i].spawn_time / 1000);
        sv_client_send_chat(i, msg);
      }
    } else {
      int range = 10;
      
      sv.edict.entities[i] = SV_ES_CLIENT;
      
      sv.bg.transform[i].position.x = (rand() % range) - range / 2;
      sv.bg.transform[i].position.y = 10;
      sv.bg.transform[i].position.z = (rand() % range) - range / 2;
      
      sv.bg.motion[i] = (bg_motion_t) {0};
    }
  }
}

#define SV_CLIENT_SHOOT (BGC_CLIENT | SVC_CLIENT | SVC_ATTACK)
#define SV_SHOOT_VICTIM (BGC_TRANSFORM | BGC_CAPSULE)
void sv_client_shoot()
{
  vec3_t forward = vec3_init(0.0f, 0.0f, 1.0f);
  
  for (int i = 0; i < sv.edict.num_entities; i++) {
    if ((sv.edict.entities[i] & SV_CLIENT_SHOOT) != SV_CLIENT_SHOOT)
      continue;
    
    if (!sv.bg.client[i].usercmd.attack && !sv.attack[i].ready)
      sv.attack[i].ready = true;
    
    if (!sv.bg.client[i].usercmd.attack || !sv.attack[i].ready)
      continue;
    
    sv.attack[i].ready = false;
    
    snapshot_t *snapshot = &sv.snapshot_queue[sv.client[i].snapshot_ack % MAX_SNAPSHOT_QUEUE];
    vec3_t ray = vec3_rotate(forward, sv.bg.transform[i].rotation);
    
    for (int j = 0; j < snapshot->edict.num_entities; j++) {
      if ((snapshot->edict.entities[j] & SV_SHOOT_VICTIM) != SV_SHOOT_VICTIM)
        continue;
      
      if (i == j)
        continue;
      
      bool hit = intersect_ray_capsule(
        sv.bg.transform[i].position,
        ray,
        snapshot->sv_transform[j].position,
        &snapshot->sv_capsule[j]);
      
      if (hit) {
        sv.edict.entities[j] = SV_ES_RESPAWN;
        sv.respawn[j].spawn_time = 5000;
        
        sv.score[i].kills++;
        sv.score[j].deaths++;
        
        char msg[128];
        snprintf(msg, 128, "[SERVER] %s killed %s.", sv.client[i].name, sv.client[j].name);
        sv_send_chat(msg);
      }
    }
  }
}

bool intersect_ray_capsule(
  vec3_t              origin,
  vec3_t              ray,
  vec3_t              offset,
  const bg_capsule_t  *capsule)
{
  vec3_t delta_pos = vec3_sub(offset, origin);
  vec3_t delta_dir = vec3_normalize(delta_pos);
  
  float proj_dist = vec3_dot(delta_dir, ray);
  
  if (proj_dist > 0) {
    vec3_t normal = vec3_normalize(vec3_add(delta_dir, vec3_mulf(ray, -proj_dist)));
    float distance = vec3_dot(origin, normal);
    
    float sphere_dist = vec3_dot(normal, offset) - distance - 3 * capsule->radius;
    
    if (sphere_dist < 0.0f)
      return true;
  }
  
  return false;
}
