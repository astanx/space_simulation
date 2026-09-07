#pragma once

class Scene;

class Validator
{
public:
  Validator() = default;
  virtual ~Validator() = default;

  virtual void init(Scene &scene, double elapsedTime) = 0;
  virtual void update(Scene &scene, double elapsedTime) = 0;
};