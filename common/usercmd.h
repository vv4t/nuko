#ifndef USERCMD_H
#define USERCMD_H

typedef struct {
  float forward;
  float right;
  float yaw;
  float pitch;
  bool jump;
  bool shoot;
} usercmd_t;

#endif
