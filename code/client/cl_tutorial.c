#include "cl_local.h"

static const vec3_t spawn_pos[] = {
  { 0.0, 1.5, 13.0    },
  { 0.0, 8.5, -15.0   },
  { -18.0, 17.0, 18.0 },
  { 15.0, 1.5, -18.0  },
  { 19.0, 11.0, 9.0   },
  { 0.0, 25, 0.0      }
};

static const int num_spawn_pos = sizeof(spawn_pos) / sizeof(vec3_t);

void cl_tutorial_init()
{
  printf("[PRACTICE] Welcome to practice\n");
  printf("[PRACTICE] Here you can experiment and practice\n");
  printf("[PRACTICE] The timer begins on your first kill and ends with your last\n");
  printf("[PRACTICE] Check out the HELP page for further details\n");
  printf("[PRACTICE] Type !connect when you're ready to play\n");
  
  cl_tutorial_spawn_player();
  cl_tutorial_spawn_dummy();
  
  cl_load_map("nk_chito");
}

void cl_tutorial_spawn_player()
{
  cg.ent_client = edict_add_entity(&cg.edict, BG_ES_CLIENT & (~BGC_MODEL));
  cg.bg.transform[cg.ent_client].position     = vec3_init(0.0f, 1.0f, 0.0f);
  cg.bg.motion[cg.ent_client]                 = (bg_motion_t) {0};
  
  cg.bg.capsule[cg.ent_client].radius         = 0.5f;
  cg.bg.capsule[cg.ent_client].height         = 1.0f;

  cg.bg.health[cg.ent_client].max             = 100;
  cg.bg.health[cg.ent_client].now             = cg.bg.health[cg.ent_client].max;
  
  cg.bg.model[cg.ent_client]                  = BG_MDL_SKULL;
  cg.bg.weapon[cg.ent_client]                 = BG_WEAPON_PISTOL;
  
  cg.bg.attack[cg.ent_client].active          = false;
  cg.bg.attack[cg.ent_client].next_attack1    = 0;
  cg.bg.attack[cg.ent_client].next_attack2    = 0;
}

void cl_tutorial_spawn_dummy()
{
  for (int i = 0; i < num_spawn_pos; i++) {
    float rand_theta = (float) (rand() % 360) * M_PI / 180.0;
    
    entity_t dummy = edict_add_entity(&cg.edict, BGC_TRANSFORM | BGC_CAPSULE | BGC_HEALTH | CLC_DUMMY | BGC_MODEL | BGC_PARTICLE);
    cg.bg.transform[dummy].position     = spawn_pos[i];
    cg.bg.transform[dummy].rotation     = quat_init_rotation(vec3_init(0.0f, 1.0f, 0.0f), rand_theta);
    
    cg.bg.capsule[dummy].radius         = 0.5f;
    cg.bg.capsule[dummy].height         = 1.0f;
    
    cg.bg.health[dummy].max             = 100;
    cg.bg.health[dummy].now             = cg.bg.health[cg.ent_client].max;
    
    cg.bg.model[dummy]                  = BG_MDL_SKULL;
  }
}

#define CL_TUTORIAL_ACTIVE_DUMMY (CLC_DUMMY)
void cl_tutorial_round()
{
  int dummy_count = 0;
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & CL_TUTORIAL_ACTIVE_DUMMY) == CL_TUTORIAL_ACTIVE_DUMMY)
      dummy_count++;
  }
  
  if (cg.round_time <= 0 && dummy_count > 0) {
    if (dummy_count < num_spawn_pos)
      cg.round_time += BG_TIMESTEP * 1000;
  } else if (dummy_count == 0) {
    printf("[PRACTICE] You finished with a time of %.2fs\n", cg.round_time / 1000.0);
    cg.round_time = 0;
    cl_tutorial_spawn_dummy();
  } else {
    cg.round_time += BG_TIMESTEP * 1000;
  }
}

#define CL_TUTORIAL_DUMMY_DEATH (BGC_HEALTH | CLC_DUMMY)
void cl_tutorial_dummy_death()
{
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & CL_TUTORIAL_DUMMY_DEATH) != CL_TUTORIAL_DUMMY_DEATH)
      continue;
    
    if (cg.bg.health[i].now <= 0) {
      if ((cg.edict.entities[i] & BGC_MODEL) == BGC_MODEL) {
        cg.bg.particle[i].now_time = 0;
        cg.bg.particle[i].end_time = 900;
        
        cg.edict.entities[i] &= ~(BGC_MODEL);
      } else if (cg.bg.particle[i].now_time > cg.bg.particle[i].end_time) {
        edict_remove_entity(&cg.edict, i);
      }
    }
  }
}

#define CL_TUTORIAL_ATTACK (BGC_ATTACK | BGC_CLIENT)
#define CL_ATTACK_VICTIM (BGC_TRANSFORM | BGC_CAPSULE | CLC_DUMMY)
void cl_tutorial_attack()
{
  vec3_t forward = vec3_init(0.0f, 0.0f, 1.0f);
  
  for (int i = 0; i < cg.edict.num_entities; i++) {
    if ((cg.edict.entities[i] & CL_TUTORIAL_ATTACK) != CL_TUTORIAL_ATTACK)
      continue;
    
    if (!cg.bg.attack[i].active)
      continue;
    
    vec3_t weap_dir = vec3_rotate(forward, cg.bg.transform[i].rotation);
    
    for (int j = 0; j < cg.edict.num_entities; j++) {
      if ((cg.edict.entities[j] & CL_ATTACK_VICTIM) != CL_ATTACK_VICTIM)
        continue;
      
      if (i == j)
        continue;
      
      bool hit = weapon_attacks[cg.bg.weapon[i]](
        cg.bg.transform[i].position,
        weap_dir,
        cg.bg.transform[j].position,
        &cg.bg.capsule[j]);
      
      if (hit)
        cg.bg.health[j].now -= weapon_attribs[cg.bg.weapon[i]].damage;
    }
  }
}
