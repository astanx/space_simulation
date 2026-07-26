#pragma once

#include "compute/clBuffer.h"

struct SharedGPUData
{
  CLBuffer positionsBuffer;
  CLBuffer orientationsBuffer;

  CLBuffer meanRadiiBuffer;
  CLBuffer polarRadiiBuffer;
  CLBuffer equatorianRadiiBuffer;
};