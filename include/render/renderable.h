#pragma once

#include "graphics/model.h"

class Shader;

class Renderable
{
protected:
  std::unique_ptr<Model> model;

public:
  virtual ~Renderable() = default;

  virtual void update(double dt) = 0;
  virtual void render(Shader &shader) = 0;

  virtual void addModel(std::unique_ptr<Model> m) { model = std::move(m); }
  Model *getModel() const { return model.get(); }
};