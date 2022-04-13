#include "texture.h"

#include "../common/sys.h"
#include <SDL2/SDL_image.h>

static GLuint load_gl_texture(void *data, int width, int height)
{
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  return texture;
}

bool texture_load(texture_t *texture, const char *path)
{
  SDL_Surface *bitmap = IMG_Load(path);
  
  if (!bitmap) {
    sys_log(SYS_ERROR, "texture_load(): could not load %s", path);
    return false;
  }
  
  *texture = load_gl_texture(bitmap->pixels, bitmap->w, bitmap->h);
  
  return true;
}

void texture_bind(texture_t texture)
{
  glBindTexture(GL_TEXTURE_2D, texture);
}
