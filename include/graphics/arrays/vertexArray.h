#pragma once

#include <GL/glew.h>

class VertexArray
{
protected:
  GLuint id = 0;

public:
  VertexArray();
  ~VertexArray();

  inline GLuint getId() const { return this->id; };

  void bind() const;
  void unbind() const;
};