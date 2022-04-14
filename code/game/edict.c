#include "edict.h"

void edict_init(edict_t *edict)
{
  for (int i = 0; i < MAX_ENTITIES; i++)
    edict->entities[i] = 0;
  
  edict->num_entities = 0;
}

entity_t edict_add_entity(edict_t *edict, int state)
{
  for (int i = 0; i < edict->num_entities; i++) {
    if (!edict->entities[i]) {
      edict->entities[i] = state;
      return i;
    }
  }
  
  edict->entities[edict->num_entities] = state;
  
  return edict->num_entities++;
}
