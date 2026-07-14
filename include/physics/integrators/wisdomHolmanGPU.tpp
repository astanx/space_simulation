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
#include <iostream>

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

  Logger::logInfo("Integrator", "calculated total");
  this->total = objects.size();

  for (System *sys : systemPointers)
    this->total += sys->getTotalObjects();

  DataGPU objectGPU;
  DataGPU orbitalGPU;

  this->objectTotal = this->total - this->orbitalTotal;
  objectGPU.resize(this->objectTotal);
  orbitalGPU.resize(this->orbitalTotal);

  size_t orbitalOffset = 0;
  size_t objectOffset = 0;

  std::mutex loveMutex;
  std::mutex tidalMutex;
  std::vector<Real> loveNumbers;
  std::vector<Real> tidalFactors;

  for (OrbitalObject *obj : orbitalObjectPointers)
    this->processOrbital(obj, orbitalGPU, orbitalOffset++, loveNumbers, tidalFactors, loveMutex, tidalMutex);

  for (Object *obj : objectPointers)
    this->processObject(obj, objectGPU, objectOffset++, loveNumbers, tidalFactors, loveMutex, tidalMutex);

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
  Logger::logInfo("Integrator", "processes obj");

  std::atomic_size_t orbitalIndex{orbitalOffset};
  std::atomic_size_t objectIndex{objectOffset};
  for (System *sys : systemPointers)
    sys->forEachObject([this, &orbitalGPU, &objectGPU, &orbitalIndex, &objectIndex, &loveNumbers, &tidalFactors, &loveMutex, &tidalMutex, &objectPointers, &orbitalObjectPointers](Object &obj)
                       {
      OrbitalObject* orb = dynamic_cast<OrbitalObject*>(&obj);
      if (orb)
      {
        size_t idx = orbitalIndex.fetch_add(1);
        this->processOrbital(orb, orbitalGPU, idx, loveNumbers, tidalFactors, loveMutex, tidalMutex);
        Object* central = orb->getOrbit()->getCentralBody();
        for (size_t j = 0; j < objectPointers.size(); j++)
        {
          if (objectPointers[j] == central)
            orbitalGPU.centralBodyIndices[idx] = j + this->orbitalTotal;
        }

        for (size_t j = 0; j < orbitalObjectPointers.size(); j++)
        {
          if (orbitalObjectPointers[j] == central)
            orbitalGPU.centralBodyIndices[idx] = j;
        }
      }
      else
      { 
        size_t idx = objectIndex.fetch_add(1);
        this->processObject(&obj, objectGPU, idx, loveNumbers, tidalFactors, loveMutex, tidalMutex); 
      } });

  Logger::logInfo("Integrator", "processed sys");

  // orbital MUST be first
  orbitalGPU.combine(objectGPU);

  Logger::logInfo("Integrator", "combined");

  cl_context ctx = context.get();
  this->positionsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.positions.size() * sizeof(Vec3), orbitalGPU.positions.data());
  this->musBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.mus.size() * sizeof(Real), orbitalGPU.mus.data());
  this->velocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.velocities.size() * sizeof(Vec3), orbitalGPU.velocities.data());
  this->accelerationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.accelerations.size() * sizeof(Vec3), orbitalGPU.accelerations.data());
  this->meanRadiiBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.meanRadii.size() * sizeof(Real), orbitalGPU.meanRadii.data());

  this->orientationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.orientations.size() * sizeof(Mat3), orbitalGPU.orientations.data());
  this->angularVelocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.angularVelocities.size() * sizeof(Vec3), orbitalGPU.angularVelocities.data());

  this->semiAxisesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.semiAxises.size() * sizeof(Real), orbitalGPU.semiAxises.data());
  this->eccentricitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.eccentricities.size() * sizeof(Real), orbitalGPU.eccentricities.data());
  this->inclinationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.inclinations.size() * sizeof(Real), orbitalGPU.inclinations.data());
  this->longitudeBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.longitude.size() * sizeof(Real), orbitalGPU.longitude.data());
  this->periapsisBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.periapsis.size() * sizeof(Real), orbitalGPU.periapsis.data());
  this->meanAnomalyBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.meanAnomaly.size() * sizeof(Real), orbitalGPU.meanAnomaly.data());
  this->meanMotionBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, orbitalGPU.meanMotion.size() * sizeof(Real), orbitalGPU.meanMotion.data());
  this->centralBodyIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.centralBodyIndices.size() * sizeof(int), orbitalGPU.centralBodyIndices.data());

  this->tensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.tensors.size() * sizeof(Mat3), orbitalGPU.tensors.data());
  this->loveIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.loveIndices.size() * sizeof(int), orbitalGPU.loveIndices.data());
  this->tidalFactorIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, orbitalGPU.tidalFactorIndices.size() * sizeof(int), orbitalGPU.tidalFactorIndices.data());
  this->loveNumbersBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, loveNumbers.size() * sizeof(Real), loveNumbers.data());
  this->tidalFactorsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, tidalFactors.size() * sizeof(Real), tidalFactors.data());
}

template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::processObject(Object *obj, DataGPU &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex)
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

  data.loveIndices[i] = -1;
  data.tidalFactorIndices[i] = -1;

  const TidalParameters &p = obj->getTidalParameters();
  if (p.k2 != -1)
  {
    std::lock_guard<std::mutex> lock(loveMutex);

    loveNumbers.push_back(static_cast<Real>(p.k2));
    data.loveIndices[i] = loveNumbers.size() - 1;
  }
  if (p.Q != -1)
  {
    std::lock_guard<std::mutex> lock(tidalMutex);

    tidalFactors.push_back(static_cast<Real>(p.Q));
    data.tidalFactorIndices[i] = tidalFactors.size() - 1;
  }
};

template <typename Real>
void WisdomHolmanIntegratorGPU<Real>::processOrbital(OrbitalObject *obj, DataGPU &data, size_t i, std::vector<Real> &loveNumbers, std::vector<Real> &tidalFactors, std::mutex &loveMutex, std::mutex &tidalMutex)
{
  this->processObject(obj, data, i, loveNumbers, tidalFactors, loveMutex, tidalMutex);

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
  this->queue.enqueueNDKernelBuffer(this->driftObjectsLinearKernel.get(), 1, &this->orbitalTotal, &this->objectTotal);
  this->queue.enqueueNDKernelBuffer(this->driftOrbitalLinearKernel.get(), 1, NULL, &this->orbitalTotal);
  this->queue.enqueueNDKernelBuffer(this->driftAngularKernel.get(), 1, NULL, &this->total);

  // Kick
  this->queue.enqueueNDKernelBuffer(this->halfKickLinearKernel.get(), 1, NULL, &this->total);
  this->queue.enqueueNDKernelBuffer(this->halfKickAngularKernel.get(), 1, NULL, &this->total, &lastEvent);

  this->events.push(lastEvent);
}