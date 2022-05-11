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
  
  if (edict->num_entities + 1 >= MAX_ENTITIES)
    return -1;
  
  return edict->num_entities++;
}

void edict_remove_entity(edict_t *edict, entity_t entity)
{
  edict->entities[entity] = 0;
  
  if (entity == edict->num_entities - 1)
    edict->num_entities--;
}
