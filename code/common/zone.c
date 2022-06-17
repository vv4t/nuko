#include "zone.h"

// TODO: add debugging features

void *zone_alloc(int size)
{
  return malloc(size);
}

void zone_free(void *ptr)
{
  free(ptr);
}
