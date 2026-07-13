#pragma once

#include "debug/logger.h"

#include "physics/integrators/integratable.h"

#include "physics/systems/system.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/constants.h"

#include "render/modelSource.h"

#include "compute/context.h"
#include "compute/commandQueue.h"

#include "resources/resourceManager.h"
#include "resources/resources.h"

#include <OpenCL/cl.h>

// Private functions
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::initKernels()
{
  this->kernelTotal = static_cast<int>(this->total);

  this->halfKickLinearKernel.setArg(0, this->positionsBuffer.get());
  this->halfKickLinearKernel.setArg(1, this->musBuffer.get());
  this->halfKickLinearKernel.setArg(2, this->velocitiesBuffer.get());
  this->halfKickLinearKernel.setArg(3, this->accelerationsBuffer.get());
  this->halfKickLinearKernel.setArg(4, this->centralBodyIndicesBuffer.get());
  this->halfKickLinearKernel.setArg(5, sizeof(int), &this->kernelTotal);

  this->halfKickAngularKernel.setArg(0, this->positionsBuffer.get());
  this->halfKickAngularKernel.setArg(1, this->musBuffer.get());
  this->halfKickAngularKernel.setArg(2, this->velocitiesBuffer.get());
  this->halfKickAngularKernel.setArg(3, this->angularVelocitiesBuffer.get());
  this->halfKickAngularKernel.setArg(4, this->tensorsBuffer.get());
  this->halfKickAngularKernel.setArg(5, this->meanRadiiBuffer.get());
  this->halfKickAngularKernel.setArg(6, this->loveIndicesBuffer.get());
  this->halfKickAngularKernel.setArg(7, this->tidalFactorIndicesBuffer.get());
  this->halfKickAngularKernel.setArg(8, this->loveNumbersBuffer.get());
  this->halfKickAngularKernel.setArg(9, this->tidalFactorsBuffer.get());
  this->halfKickAngularKernel.setArg(10, sizeof(int), &this->kernelTotal);

  this->driftOrbitalLinearKernel.setArg(0, this->positionsBuffer.get());
  this->driftOrbitalLinearKernel.setArg(1, this->musBuffer.get());
  this->driftOrbitalLinearKernel.setArg(2, this->velocitiesBuffer.get());
  this->driftOrbitalLinearKernel.setArg(3, this->semiAxisesBuffer.get());
  this->driftOrbitalLinearKernel.setArg(4, this->eccentricitiesBuffer.get());
  this->driftOrbitalLinearKernel.setArg(5, this->inclinationsBuffer.get());
  this->driftOrbitalLinearKernel.setArg(6, this->longitudeBuffer.get());
  this->driftOrbitalLinearKernel.setArg(7, this->periapsisBuffer.get());
  this->driftOrbitalLinearKernel.setArg(8, this->meanAnomalyBuffer.get());
  this->driftOrbitalLinearKernel.setArg(9, this->meanMotionBuffer.get());
  this->driftOrbitalLinearKernel.setArg(10, this->centralBodyIndicesBuffer.get());

  this->driftObjectsLinearKernel.setArg(0, this->positionsBuffer.get());
  this->driftObjectsLinearKernel.setArg(1, this->velocitiesBuffer.get());

  this->driftAngularKernel.setArg(0, this->velocitiesBuffer.get());
  this->driftAngularKernel.setArg(1, this->angularVelocitiesBuffer.get());
  this->driftAngularKernel.setArg(2, this->orientationsBuffer.get());
}
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::initQueues(Context &ctx)
{
  this->queue.init(ctx.get(), ctx.getDevice());
}
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::initBuffers(std::vector<Integratable *> &objects, Context &context)
{
  std::vector<Object *> objectPointers;
  std::vector<OrbitalObject *> orbitalObjectPointers;
  std::vector<System *> systemPointers;

  this->splitObjectsSystems(objects, objectPointers, orbitalObjectPointers, systemPointers);

  this->orbitalTotal = orbitalObjectPointers.size();
  for (System *sys : systemPointers)
  {
    std::vector<bool> isOrbital(sys->getTotalObjects());

    sys->forEachObject([&](Object &obj, size_t i)
                       { isOrbital[i] = dynamic_cast<OrbitalObject *>(&obj) != nullptr; });
    this->orbitalTotal += std::count(isOrbital.begin(), isOrbital.end(), 1);
  }

  this->total = objects.size();
  for (System *sys : systemPointers)
    this->total += sys->getTotalObjects();

  DataGPU objectGPU;
  DataGPU orbitalGPU;

  objectGPU.resize(this->total - this->orbitalTotal);
  orbitalGPU.resize(this->orbitalTotal);

  size_t orbitalOffset = 0;
  size_t objectOffset = 0;

  std::mutex loveMutex;
  std::mutex tidalMutex;

  for (OrbitalObject *obj : orbitalObjectPointers)
    this->processOrbital(obj, orbitalGPU, orbitalOffset++, loveMutex, tidalMutex);

  for (Object *obj : objectPointers)
    this->processObject(obj, objectGPU, objectOffset++, loveMutex, tidalMutex);

  for (size_t i = 0; i < orbitalObjectPointers.size(); i++)
  {
    Object *central = orbitalObjectPointers[i]->getOrbit()->getCentralBody();
    for (size_t j = 0; j < objectPointers.size(); j++)
    {
      if (objectPointers[j] == central)
        orbitalGPU.centralBodyIndices[i] = j + this->orbitalTotal;
    }

    for (size_t j = 0; j < orbitalObjectPointers.size(); j++)
    {
      if (orbitalObjectPointers[j] == central)
        orbitalGPU.centralBodyIndices[i] = j;
    }
  }

  for (System *sys : systemPointers)
    sys->forEachObject([this, &orbitalGPU, &objectGPU, orbitalOffset, objectOffset, &loveMutex, &tidalMutex, &objectPointers, &orbitalObjectPointers](Object &obj, size_t i)
                       {
      OrbitalObject* orb = dynamic_cast<OrbitalObject*>(&obj);
      if (orb)
      {
        this->processOrbital(orb, orbitalGPU, orbitalOffset + i, loveMutex, tidalMutex);
        Object* central = orb->getOrbit()->getCentralBody();
        for (size_t j = 0; j < objectPointers.size(); j++)
        {
          if (objectPointers[j] == central)
            orbitalGPU.centralBodyIndices[orbitalOffset + i] = j + this->orbitalTotal;
        }

        for (size_t j = 0; j < orbitalObjectPointers.size(); j++)
        {
          if (orbitalObjectPointers[j] == central)
            orbitalGPU.centralBodyIndices[orbitalOffset + i] = j;
        }
      }
      else
        this->processObject(&obj, objectGPU, objectOffset + i, loveMutex, tidalMutex); });

  // orbital MUST be first
  orbitalGPU.combine(objectGPU);
  DataGPU finalGPU = orbitalGPU;

  cl_context ctx = context.get();
  this->positionsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.positions.size() * sizeof(Vec3), finalGPU.positions.data());
  this->musBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, finalGPU.mus.size() * sizeof(Real), finalGPU.mus.data());
  this->velocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.velocities.size() * sizeof(Vec3), finalGPU.velocities.data());
  this->accelerationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.accelerations.size() * sizeof(Vec3), finalGPU.accelerations.data());
  this->meanRadiiBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, finalGPU.meanRadii.size() * sizeof(Real), finalGPU.meanRadii.data());

  this->orientationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.orientations.size() * sizeof(Mat3), finalGPU.orientations.data());
  this->angularVelocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.angularVelocities.size() * sizeof(Vec3), finalGPU.angularVelocities.data());

  this->semiAxisesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.semiAxises.size() * sizeof(Real), finalGPU.semiAxises.data());
  this->eccentricitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.eccentricities.size() * sizeof(Real), finalGPU.eccentricities.data());
  this->inclinationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.inclinations.size() * sizeof(Real), finalGPU.inclinations.data());
  this->longitudeBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.longitude.size() * sizeof(Real), finalGPU.longitude.data());
  this->periapsisBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.periapsis.size() * sizeof(Real), finalGPU.periapsis.data());
  this->meanAnomalyBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.meanAnomaly.size() * sizeof(Real), finalGPU.meanAnomaly.data());
  this->meanMotionBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.meanMotion.size() * sizeof(Real), finalGPU.meanMotion.data());
  this->centralBodyIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, finalGPU.centralBodyIndices.size() * sizeof(int), finalGPU.centralBodyIndices.data());

  this->tensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, finalGPU.tensors.size() * sizeof(Mat3), finalGPU.tensors.data());
  this->loveIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, finalGPU.loveIndices.size() * sizeof(int), finalGPU.loveIndices.data());
  this->tidalFactorIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, finalGPU.tidalFactorIndices.size() * sizeof(int), finalGPU.tidalFactorIndices.data());
  this->loveNumbersBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.loveNumbers.size() * sizeof(Real), finalGPU.loveNumbers.data());
  this->tidalFactorsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, finalGPU.tidalFactors.size() * sizeof(Real), finalGPU.tidalFactors.data());
}

template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::processObject(Object *obj, DataGPU &data, size_t i, std::mutex &loveMutex, std::mutex &tidalMutex)
{
  data.positions[i] = static_cast<Vec3>(obj->getPosition());
  data.velocities[i] = static_cast<Vec3>(obj->getVelocity());
  data.accelerations[i] = static_cast<Vec3>(obj->getAcceleration());
  data.mus[i] = static_cast<Real>(obj->getMu());
  data.meanRadii[i] = static_cast<Real>(obj->getRadius());

  data.orientations[i] = static_cast<Mat3>(obj->getOrientation());
  data.angularVelocities[i] = static_cast<Vec3>(obj->getAngularVelocity());

  data.centralBodyIndices[i] = -1;

  data.tensors[i] = static_cast<Mat3>(obj->getQuadrupoleTensor());

  const TidalParameters &p = obj->getTidalParameters();
  if (p.k2 != -1)
  {
    std::lock_guard<std::mutex> lock(loveMutex);

    data.loveNumbers.push_back(static_cast<Real>(p.k2));
    data.loveIndices.push_back(data.loveNumbers.size() - 1);
  }
  if (p.Q != -1)
  {
    std::lock_guard<std::mutex> lock(tidalMutex);

    data.tidalFactors.push_back(static_cast<Real>(p.Q));
    data.tidalFactorIndices.push_back(data.tidalFactors.size() - 1);
  }
};

template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::processOrbital(OrbitalObject *obj, DataGPU &data, size_t i, std::mutex &loveMutex, std::mutex &tidalMutex)
{
  this->processObject(obj, data, i, loveMutex, tidalMutex);

  const KeplerElements k = obj->getOrbit()->getKeplerElements();

  data.semiAxises[i] = static_cast<Real>(k.a);
  data.eccentricities[i] = static_cast<Real>(k.e);
  data.inclinations[i] = static_cast<Real>(k.i);
  data.longitude[i] = static_cast<Real>(k.Omega);
  data.periapsis[i] = static_cast<Real>(k.omega);
  data.meanAnomaly[i] = static_cast<Real>(k.m);
  data.meanMotion[i] = static_cast<Real>(k.n);
};

template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::updateDt(Real dt)
{
  Real kickDt = dt * 0.5;
  this->halfKickLinearKernel.setArg(6, sizeof(Real), &kickDt);
  this->halfKickAngularKernel.setArg(11, sizeof(Real), &kickDt);
  this->driftOrbitalLinearKernel.setArg(11, sizeof(Real), &dt);
  this->driftObjectsLinearKernel.setArg(2, sizeof(Real), &dt);
  this->driftAngularKernel.setArg(3, sizeof(Real), &dt);
}

// Constructor
template <typename Real>
WisdomHolmanIntegratorGPU<Real>::WisdomHolmanIntegratorGPU(ResourceManager &resourceManager)
    : driftAngularKernel(resourceManager.GetKernel(Res::DRIFT_ANGULAR_KERNEL)),
      driftObjectsLinearKernel(resourceManager.GetKernel(Res::DRIFT_OBJECTS_LINEAR_KERNEL)),
      driftOrbitalLinearKernel(resourceManager.GetKernel(Res::DRIFT_ORBITAL_LINEAR_KERNEL)),
      halfKickLinearKernel(resourceManager.GetKernel(Res::HALF_KICK_LINEAR_KERNEL)),
      halfKickAngularKernel(resourceManager.GetKernel(Res::HALF_KICK_ANGULAR_KERNEL))
{
}

// Public functions
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::init(std::vector<Integratable *> &objects, Context &ctx)
{
  this->initBuffers(objects, ctx);
  this->initQueues(ctx);
  this->initKernels();
}
template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::stepReal(Real dt)
{
  this->updateDt(dt);

  if (this->events.size() >= 2)
  {
    cl_event event = this->events.front();
    this->events.pop();
    clWaitForEvents(1, &event);
    clReleaseEvent(event);
  }

  cl_event lastEvent;

  // Kick
  this->queue.enqueueNDKernelBuffer(this->halfKickLinearKernel.get(), 1, NULL, &this->total);
  this->queue.enqueueNDKernelBuffer(this->halfKickAngularKernel.get(), 1, NULL, &this->total);

  // Drift
  this->queue.enqueueNDKernelBuffer(this->driftObjectsLinearKernel.get(), 1, &this->orbitalTotal, &this->total);
  this->queue.enqueueNDKernelBuffer(this->driftOrbitalLinearKernel.get(), 1, NULL, &this->orbitalTotal);
  this->queue.enqueueNDKernelBuffer(this->driftAngularKernel.get(), 1, NULL, &this->total);

  // Kick
  this->queue.enqueueNDKernelBuffer(this->halfKickLinearKernel.get(), 1, NULL, &this->total);
  this->queue.enqueueNDKernelBuffer(this->halfKickAngularKernel.get(), 1, NULL, &this->total, &lastEvent);

  this->events.push(lastEvent);
}