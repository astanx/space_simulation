#pragma once

#include <GL/glew.h>

class RenderBuffer
{
protected:
  GLuint id = 0;

public:
  RenderBuffer();
  ~RenderBuffer();

  inline GLuint getId() const { return this->id; };

  void bind(GLenum target) const;
  void unbind(GLenum target) const;
};