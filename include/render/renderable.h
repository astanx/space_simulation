#pragma once

#include <cstddef>

class Shader;
class Buffer;

class Renderable
{
protected:
public:
  Renderable() = default;
  virtual ~Renderable() = default;

  virtual void render(Shader &shader) = 0;
  virtual void renderInstanced(Shader &shader, Buffer *instanceVBO = nullptr, size_t size = 0, size_t count = 0, size_t offset = 0) = 0;
};