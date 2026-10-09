#pragma once

enum class PhysicsIntegrator
{
  WisdomHolman,
  Euler,
  RK4,
};

enum class PhysicsForceModel
{
  Direct,
  BarnesHut,
};

struct PhysicsConfig
{
  PhysicsForceModel forceModel = PhysicsForceModel::Direct;
  PhysicsIntegrator integrator = PhysicsIntegrator::WisdomHolman;
};