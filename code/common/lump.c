#include "lump.h"

void *lump_load(FILE *file, const lump_t *lump, int stride, int *count)
{
  // Allocate a zone buffer
  void *data = zone_alloc(lump->filelen);
  
  // Read data from the file
  fseek(file, lump->fileofs, SEEK_SET);
  fread(data, lump->filelen, 1, file);
  
  // Calculate how many elements are in the lump
  if (count)
    *count = lump->filelen / stride;
  
  return data;
}
