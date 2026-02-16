#pragma once

#include "graphics/texture.h"

#include <GL/glew.h>

class ScopedTexture
{
private:
  GLint prevTexture = 0;
  GLint prevActiveTexture = 0;
  GLenum target = 0;
  GLint unit = 0;

  static GLenum getBindingEnumForTarget(GLenum target);

public:
  explicit ScopedTexture(const Texture &texture, GLint unit, bool saveState = false);
  explicit ScopedTexture(GLuint id, GLenum target, GLint unit, bool saveState = false);
  ~ScopedTexture();
};