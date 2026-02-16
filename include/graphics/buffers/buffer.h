#pragma once

#include <GL/glew.h>

class Buffer
{
protected:
  GLuint id = 0;

public:
  Buffer() = default;
  ~Buffer() = default;

  inline GLuint getId() const { return this->id; };

  virtual void bind(GLenum target) const = 0;
  virtual void unbind(GLenum target) const = 0;

  virtual bool checkComplete() const = 0;
};