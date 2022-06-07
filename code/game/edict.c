#include "edict.h"

void edict_init(edict_t *edict)
{
  // Reset all entities
  for (int i = 0; i < MAX_ENTITIES; i++)
    edict->entities[i] = 0;
  
  edict->num_entities = 0;
}

entity_t edict_add_entity(edict_t *edict, int state)
{
  // Find an empty entity slot within already "allocated section"
  for (int i = 0; i < edict->num_entities; i++) {
    if (!edict->entities[i]) { // This is simply if the entity state is '0' i.e. no components
      edict->entities[i] = state;
      return i;
    }
  }
  
  // If there aren't any, add a new one
  edict->entities[edict->num_entities] = state;
  
  // Boundary checks
  if (edict->num_entities + 1 >= MAX_ENTITIES)
    return -1;
  
  return edict->num_entities++;
}

void edict_remove_entity(edict_t *edict, entity_t entity)
{
  // This achieved by setting the state to '0' marking it as empty
  edict->entities[entity] = 0;
  
  // If it was at the end, reduce the "allocated section"
  if (entity == edict->num_entities - 1)
    edict->num_entities--;
}
