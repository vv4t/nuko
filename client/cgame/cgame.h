#ifndef CGAME_H
#define CGAME_H

class cgame_t {
private:
  edict_t       m_edict;
  c_transform_t m_c_transform[MAX_ENTITIES];
  c_motion_t    m_c_motion[MAX_ENTITIES];
  c_client_t    m_c_client[MAX_ENTITIES];

public:
  cgame_t();
  
  void update();
};

#endif
