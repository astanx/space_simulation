#pragma once

#include <GL/glew.h>

class ScopedDepthTest
{
private:
  GLboolean wasEnabled;

public:
  explicit ScopedDepthTest(bool enable = true);
  ~ScopedDepthTest();
};