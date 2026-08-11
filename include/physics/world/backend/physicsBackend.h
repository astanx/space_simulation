#pragma once

class PhysicsBackend
{
public:
  PhysicsBackend() = default;
  virtual ~PhysicsBackend() = default;

  virtual void step(double dt) = 0;
};