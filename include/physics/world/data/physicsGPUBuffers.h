#pragma once

#include "compute/clBuffer.h"

struct PhysicsGPUBuffers
{
  CLBuffer musBuffer;
  CLBuffer velocitiesBuffer;

  CLBuffer angularVelocitiesBuffer;

  CLBuffer quadrupoleTensorsBuffer;
  CLBuffer inertiaTensorsBuffer;
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
};