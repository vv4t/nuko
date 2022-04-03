#ifndef EDICT_H
#define EDICT_H

#define MAX_ENTIITES 32

class edict_t {
private:
  int m_entities[MAX_ENTITIES];
  int m_num_entities;

public:
  int add_entity(int mask);
};

#endif
