#pragma once

class Scene;

class Validator
{
public:
  Validator() = default;
  virtual ~Validator() = default;

  virtual void init(Scene &scene, double elapsedTime, size_t steps) = 0;
  virtual void update(Scene &scene, double elapsedTime) = 0;
};