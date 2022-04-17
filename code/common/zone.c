#include "zone.h"

#include <stdlib.h>

void *zone_alloc(int size)
{
  return malloc(size);
}

void zone_free(void *ptr)
{
  free(ptr);
}
