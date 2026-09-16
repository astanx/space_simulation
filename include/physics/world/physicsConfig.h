#pragma once

enum class PhysicsIntegrator
{
  WisdomHolman,
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