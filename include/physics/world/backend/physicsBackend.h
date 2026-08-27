#pragma once

template <typename Real>
class PhysicsBackend
{
public:
  PhysicsBackend() = default;
  virtual ~PhysicsBackend() = default;

  virtual void step(Real dt) = 0;
};