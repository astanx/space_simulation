#pragma once

#include "compute/clBuffer.h"

#include "physics/integrators/integratorGPUData.h"
#include "physics/world/data/physicsGPUData.h"
#include "scene/world/data/sharedGPUData.h"

struct WisdomHolmanGPUData : IntegratorGPUData
{
  CLBuffer &positionsBuffer;
  CLBuffer &orientationsBuffer;

  CLBuffer &meanRadiiBuffer;
  CLBuffer &polarRadiiBuffer;
  CLBuffer &equatorianRadiiBuffer;

  CLBuffer &musBuffer;
  CLBuffer &velocitiesBuffer;

  CLBuffer &angularVelocitiesBuffer;

  CLBuffer &quadrupoleTensorsBuffer;
  CLBuffer &inertiaTensorsBuffer;
  CLBuffer &loveIndicesBuffer;
  CLBuffer &tidalFactorIndicesBuffer;
  CLBuffer &loveNumbersBuffer;
  CLBuffer &tidalFactorsBuffer;

  CLBuffer &semiAxisesBuffer;
  CLBuffer &eccentricitiesBuffer;
  CLBuffer &inclinationsBuffer;
  CLBuffer &longitudeBuffer;
  CLBuffer &periapsisBuffer;
  CLBuffer &meanAnomalyBuffer;
  CLBuffer &meanMotionBuffer;
  CLBuffer &centralBodyIndicesBuffer;

  WisdomHolmanGPUData(PhysicsGPUData &physics, SharedGPUData &shared)
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