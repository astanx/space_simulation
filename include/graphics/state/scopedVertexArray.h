#pragma once

#include "graphics/arrays/vertexArray.h"

class ScopedVertexArray
{
private:
  GLint prevArray = 0;

public:
  explicit ScopedVertexArray(const VertexArray &array, bool saveState = false);
  explicit ScopedVertexArray(const GLint &array, bool saveState = false);

  ~ScopedVertexArray();
};