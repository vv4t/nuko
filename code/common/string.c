#include "string.h"

char *string_copy(const char *str)
{
  int len = strlen(str) + 1;
  char *new_str = zone_alloc(len); // Allocate using zone
  memcpy(new_str, str, len);
  
  return new_str;
}
