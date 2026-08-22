#pragma once

#include "compute/clBuffer.h"

#include "physics/integrators/integratorGPUBuffers.h"
#include "physics/world/data/physicsGPUBuffers.h"
#include "scene/world/data/sharedGPUBuffers.h"

struct WisdomHolmanGPUBuffers : IntegratorGPUBuffers
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

  const CLBuffer &semiAxisesBuffer;
  const CLBuffer &eccentricitiesBuffer;
  const CLBuffer &inclinationsBuffer;
  const CLBuffer &longitudeBuffer;
  const CLBuffer &periapsisBuffer;
  const CLBuffer &meanAnomalyBuffer;
  const CLBuffer &meanMotionBuffer;
  const CLBuffer &centralBodyIndicesBuffer;

  WisdomHolmanGPUBuffers(const PhysicsGPUBuffers &physics, const SharedGPUBuffers &shared)
      : positionsBuffer(shared.positionsBuffer),
        orientationsBuffer(shared.orientationsBuffer),

        meanRadiiBuffer(shared.meanRadiiBuffer),
        polarRadiiBuffer(shared.polarRadiiBuffer),
        equatorianRadiiBuffer(shared.equatorianRadiiBuffer),

        musBuffer(physics.musBuffer),
        velocitiesBuffer(physics.velocitiesBuffer),

        angularVelocitiesBuffer(physics.angularVelocitiesBuffer),

        quadrupoleTensorsBuffer(physics.quadrupoleTensorsBuffer),
        inertiaTensorsBuffer(physics.inertiaTensorsBuffer),
        loveIndicesBuffer(physics.loveIndicesBuffer),
        tidalFactorIndicesBuffer(physics.tidalFactorIndicesBuffer),
        loveNumbersBuffer(physics.loveNumbersBuffer),
        tidalFactorsBuffer(physics.tidalFactorsBuffer),

        semiAxisesBuffer(physics.semiAxisesBuffer),
        eccentricitiesBuffer(physics.eccentricitiesBuffer),
        inclinationsBuffer(physics.inclinationsBuffer),
        longitudeBuffer(physics.longitudeBuffer),
        periapsisBuffer(physics.periapsisBuffer),
        meanAnomalyBuffer(physics.meanAnomalyBuffer),
        meanMotionBuffer(physics.meanMotionBuffer),
        centralBodyIndicesBuffer(physics.centralBodyIndicesBuffer) {};
};