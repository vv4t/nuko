/*
-- zone.h --

Wrap memory allocation in a function for debugging purposes.
*/

#ifndef ZONE_H
#define ZONE_H

#include <stdlib.h>

void  *zone_alloc(int size);
void  zone_free(void *ptr);

#endif
