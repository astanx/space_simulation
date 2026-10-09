#pragma once

#include "physics/integrators/integratorGPU.h"

#include "graphics/buffers/buffer.h"

#include "compute/clBuffer.h"
#include "compute/commandQueue.h"
#include "compute/kernel.h"

struct EulerGPUBuffers;
class Object;
class OrbitalObject;
class Integratable;
class Context;

template <typename Real>
class EulerIntegratorGPU : public IIntegratorGPU<Real>
{
protected:
  Kernel &driftAngularKernel;
  Kernel &driftLinearKernel;

  void initKernels(EulerGPUBuffers &gpu, Total total);

  void updateDt(Real dt);

public:
  EulerIntegratorGPU(ResourceManager &resourceManager);
  ~EulerIntegratorGPU() = default;

  void init(AllIntegratorGPUBuffers &gpu, Total &total) override;

  void stepReal(CommandQueue &queue, Total &total, Real dt) override;
};

#include "physics/integrators/eulerGPU.hpp"