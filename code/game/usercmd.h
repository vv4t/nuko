#ifndef USERCMD_H
#define USERCMD_H

#include <stdbool.h>

typedef struct {
  float forward;
  float right;
  float yaw;
  float pitch;
  bool jump;
} usercmd_t;

#endif
