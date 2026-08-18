#pragma once

#include "compute/clBuffer.h"

struct SharedGPUBuffers
{
  CLBuffer positionsBuffer;
  CLBuffer orientationsBuffer;

  CLBuffer meanRadiiBuffer;
  CLBuffer polarRadiiBuffer;
  CLBuffer equatorianRadiiBuffer;
};