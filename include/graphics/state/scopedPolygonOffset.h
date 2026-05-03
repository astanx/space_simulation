#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class ScopedPolygonOffset
{
private:
  GLboolean wasEnabled;

public:
  explicit ScopedPolygonOffset(bool enable = true, float factor = 1.f, float units = 1.f);
  ~ScopedPolygonOffset();
};