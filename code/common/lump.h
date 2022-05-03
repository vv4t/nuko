#ifndef LUMP_H
#define LUMP_H

#include "zone.h"
#include <stdio.h>

typedef struct {
  int         fileofs;
  int         filelen;
} lump_t;

void *lump_load(FILE *file, const lump_t *lump, int stride, int *count);

#endif
