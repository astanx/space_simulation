#pragma once

class Shader;

class Renderable
{
protected:
public:
  Renderable() = default;
  virtual ~Renderable() = default;

  virtual void render(Shader &shader) = 0;
  virtual void renderInstanced() = 0;
};