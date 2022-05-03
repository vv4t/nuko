#include "lump.h"

void *lump_load(FILE *file, const lump_t *lump, int stride, int *count)
{
  void *data = zone_alloc(lump->filelen);
  
  fseek(file, lump->fileofs, SEEK_SET);
  fread(data, lump->filelen, 1, file);
  
  if (count)
    *count = lump->filelen / stride;
  
  return data;
}
