#pragma once

#include <GL/glew.h>

class VertexArray
{
protected:
  GLuint id = 0;

  bool layoutInitialized = false;

public:
  VertexArray();
  ~VertexArray();

  inline const GLuint getId() const { return this->id; };

  void bind() const;
  void unbind() const;

  bool isLayoutInitialized() { return this->layoutInitialized; };
  void setLayoutInitialized(bool initialized) { this->layoutInitialized = initialized; };
};