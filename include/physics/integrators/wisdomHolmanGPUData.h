#pragma once

#include "compute/clBuffer.h"

#include "physics/integrators/integratorGPUData.h"
#include "physics/world/data/physicsGPUData.h"
#include "scene/world/data/sharedGPUData.h"

struct WisdomHolmanGPUData : IntegratorGPUData
{
  CLBuffer positionsBuffer;
  CLBuffer orientationsBuffer;

  CLBuffer meanRadiiBuffer;
  CLBuffer polarRadiiBuffer;
  CLBuffer equatorianRadiiBuffer;

  CLBuffer musBuffer;
  CLBuffer velocitiesBuffer;

  CLBuffer angularVelocitiesBuffer;

  CLBuffer tensorsBuffer;
  CLBuffer loveIndicesBuffer;
  CLBuffer tidalFactorIndicesBuffer;
  CLBuffer loveNumbersBuffer;
  CLBuffer tidalFactorsBuffer;

  CLBuffer semiAxisesBuffer;
  CLBuffer eccentricitiesBuffer;
  CLBuffer inclinationsBuffer;
  CLBuffer longitudeBuffer;
  CLBuffer periapsisBuffer;
  CLBuffer meanAnomalyBuffer;
  CLBuffer meanMotionBuffer;
  CLBuffer centralBodyIndicesBuffer;

  WisdomHolmanGPUData(PhysicsGPUData physics, SharedGPUData shared)
  {
    this->positionsBuffer = shared.positionsBuffer;
    this->orientationsBuffer = shared.orientationsBuffer;

    this->meanRadiiBuffer = shared.meanRadiiBuffer;
    this->polarRadiiBuffer = shared.polarRadiiBuffer;
    this->equatorianRadiiBuffer = shared.equatorianRadiiBuffer;

    this->musBuffer = physics.musBuffer;
    this->velocitiesBuffer = physics.velocitiesBuffer;

    this->angularVelocitiesBuffer = physics.angularVelocitiesBuffer;

    this->tensorsBuffer = physics.tensorsBuffer;
    this->loveIndicesBuffer = physics.loveIndicesBuffer;
    this->tidalFactorIndicesBuffer = physics.tidalFactorIndicesBuffer;
    this->loveNumbersBuffer = physics.loveNumbersBuffer;
    this->tidalFactorsBuffer = physics.tidalFactorsBuffer;

    this->semiAxisesBuffer = physics.semiAxisesBuffer;
    this->eccentricitiesBuffer = physics.eccentricitiesBuffer;
    this->inclinationsBuffer = physics.inclinationsBuffer;
    this->longitudeBuffer = physics.longitudeBuffer;
    this->periapsisBuffer = physics.periapsisBuffer;
    this->meanAnomalyBuffer = physics.meanAnomalyBuffer;
    this->meanMotionBuffer = physics.meanMotionBuffer;
    this->centralBodyIndicesBuffer = physics.centralBodyIndicesBuffer;
  };
};