#include "file.h"

const char *file_read_all(const char *path)
{
  FILE *f = fopen(path, "rb");
  
  if (!f) {
    log_printf(LOG_ERROR, "file_read_all(): failed to read file '%s'", path);
    return NULL;
  }
  
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);
  
  char *buf = zone_alloc(fsize + 1);
  fread(buf, fsize, 1, f);
  fclose(f);
  
  buf[fsize] = 0;
  
  return buf;
}
