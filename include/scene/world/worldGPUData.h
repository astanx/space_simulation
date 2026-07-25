#pragma once

#include "compute/clBuffer.h"

struct WorldGPUData
{
  CLBuffer positionsBuffer;
  CLBuffer meanRadiiBuffer;
  CLBuffer polarRadiiBuffer;
  CLBuffer equatorianRadiiBuffer;
  CLBuffer orientationsBuffer;
};