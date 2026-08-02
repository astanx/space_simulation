#pragma once

#include "physics/integrators/integratorGPU.h"

#include "graphics/buffers/buffer.h"

#include "compute/clBuffer.h"
#include "compute/commandQueue.h"
#include "compute/kernel.h"

struct WisdomHolmanGPUData;
class Object;
class OrbitalObject;
class Integratable;
class Context;

template <typename Real>
class WisdomHolmanIntegratorGPU : public IIntegratorGPU<Real>
{
protected:
  Kernel &driftAngularKernel;
  Kernel &driftObjectsLinearKernel;
  Kernel &driftOrbitalLinearKernel;
  Kernel &halfKickLinearKernel;
  Kernel &halfKickAngularKernel;
  Kernel &halfKickKernel;

  void initKernels(WisdomHolmanGPUData &gpu, Total total);

  void updateDt(Real dt);

public:
  WisdomHolmanIntegratorGPU(ResourceManager &resourceManager);
  ~WisdomHolmanIntegratorGPU() = default;

  void init(IntegratorGPUData &gpu, Total &total, Context &ctx) override;

  void stepReal(CommandQueue &queue, Total &total, Real dt) override;
};

#include "physics/integrators/wisdomHolmanGPU.tpp"