#include "string.h"

#include "zone.h"
#include <string.h>

char *string_copy(const char *str)
{
  int len = strlen(str) + 1;
  char *new_str = zone_alloc(len);
  memcpy(new_str, str, len);
  
  return new_str;
}
