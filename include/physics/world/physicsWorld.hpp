#pragma once

#include "compute/context.h"

#include "physics/world/data/physicsDataGPU.h"

#include "resources/resourceManager.h"

// Public functions
template <typename Real>
void PhysicsWorld::initGPUBuffers(Context &context, PhysicsDataGPU<Real> data)
{
  cl_context ctx = context.get();

  this->gpu.musBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.mus.size() * sizeof(Real), data.mus.data());
  this->gpu.velocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.velocities.size() * sizeof(Vec3<Real>), data.velocities.data());

  this->gpu.angularVelocitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.angularVelocities.size() * sizeof(Vec3<Real>), data.angularVelocities.data());

  this->gpu.semiAxisesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.semiAxises.size() * sizeof(Real), data.semiAxises.data());
  this->gpu.eccentricitiesBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.eccentricities.size() * sizeof(Real), data.eccentricities.data());
  this->gpu.inclinationsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.inclinations.size() * sizeof(Real), data.inclinations.data());
  this->gpu.longitudeBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.longitude.size() * sizeof(Real), data.longitude.data());
  this->gpu.periapsisBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.periapsis.size() * sizeof(Real), data.periapsis.data());
  this->gpu.meanAnomalyBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.meanAnomaly.size() * sizeof(Real), data.meanAnomaly.data());
  this->gpu.meanMotionBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.meanMotion.size() * sizeof(Real), data.meanMotion.data());
  this->gpu.centralBodyIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.centralBodyIndices.size() * sizeof(int), data.centralBodyIndices.data());

  this->gpu.tensorsBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.tensors.size() * sizeof(Mat3<Real>), data.tensors.data());
  this->gpu.loveIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.loveIndices.size() * sizeof(int), data.loveIndices.data());
  this->gpu.tidalFactorIndicesBuffer.init(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data.tidalFactorIndices.size() * sizeof(int), data.tidalFactorIndices.data());
  this->gpu.loveNumbersBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.loveNumbers.size() * sizeof(Real), data.loveNumbers.data());
  this->gpu.tidalFactorsBuffer.init(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, data.tidalFactors.size() * sizeof(Real), data.tidalFactors.data());
}