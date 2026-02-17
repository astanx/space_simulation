#pragma once

#include <GL/glew.h>

class Buffer
{
protected:
  GLuint id = 0;

public:
  Buffer();
  ~Buffer();

  inline GLuint getId() const { return this->id; };

  void bind(GLenum target) const;
  void unbind(GLenum target) const;

  void bindBufferBase(GLenum target, GLint binding) const;
};