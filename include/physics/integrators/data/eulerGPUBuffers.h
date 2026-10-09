#pragma once

#include "compute/clBuffer.h"

#include "physics/integrators/data/integratorGPUBuffers.h"
#include "physics/integrators/data/allIntegratorGPUBuffers.h"

struct EulerGPUBuffers : IntegratorGPUBuffers
{
  const CLBuffer &positionsBuffer;
  const CLBuffer &orientationsBuffer;

  const CLBuffer &meanRadiiBuffer;
  const CLBuffer &polarRadiiBuffer;
  const CLBuffer &equatorianRadiiBuffer;

  const CLBuffer &musBuffer;
  const CLBuffer &velocitiesBuffer;

  const CLBuffer &angularVelocitiesBuffer;

  const CLBuffer &quadrupoleTensorsBuffer;
  const CLBuffer &inertiaTensorsBuffer;
  const CLBuffer &loveIndicesBuffer;
  const CLBuffer &tidalFactorIndicesBuffer;
  const CLBuffer &loveNumbersBuffer;
  const CLBuffer &tidalFactorsBuffer;

  EulerGPUBuffers(const AllIntegratorGPUBuffers &all)
      : positionsBuffer(all.positionsBuffer),
        orientationsBuffer(all.orientationsBuffer),

        meanRadiiBuffer(all.meanRadiiBuffer),
        polarRadiiBuffer(all.polarRadiiBuffer),
        equatorianRadiiBuffer(all.equatorianRadiiBuffer),

        musBuffer(all.musBuffer),
        velocitiesBuffer(all.velocitiesBuffer),

        angularVelocitiesBuffer(all.angularVelocitiesBuffer),

        quadrupoleTensorsBuffer(all.quadrupoleTensorsBuffer),
        inertiaTensorsBuffer(all.inertiaTensorsBuffer),
        loveIndicesBuffer(all.loveIndicesBuffer),
        tidalFactorIndicesBuffer(all.tidalFactorIndicesBuffer),
        loveNumbersBuffer(all.loveNumbersBuffer),
        tidalFactorsBuffer(all.tidalFactorsBuffer) {};
};