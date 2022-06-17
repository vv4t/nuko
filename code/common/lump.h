/*
-- lump.h --

Lump file formats. Lumps describe an array of fixed-size object and are usually
in the header of the file.
*/

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
