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
  CommandQueue queue;

  Kernel &driftAngularKernel;
  Kernel &driftObjectsLinearKernel;
  Kernel &driftOrbitalLinearKernel;
  Kernel &halfKickLinearKernel;
  Kernel &halfKickAngularKernel;
  Kernel &halfKickKernel;

  std::queue<cl_event> events;

  void initQueues(Context &ctx);
  void initKernels(WisdomHolmanGPUData &gpu, Total total);
  void initGLBuffers();
  void initGLBuffer(Buffer &buffer, size_t size);
  void initBuffers(std::vector<Integratable *> &objects, Context &ctx);

  void updateDt(Real dt);

public:
  WisdomHolmanIntegratorGPU(ResourceManager &resourceManager);
  ~WisdomHolmanIntegratorGPU() = default;

  void init(WisdomHolmanGPUData &gpu, Total &total, Context &ctx) override;

  void stepReal(Total &total, Real dt) override;
};

#include "physics/integrators/wisdomHolmanGPU.tpp"