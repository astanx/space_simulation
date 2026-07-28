#pragma once

#include "compute/commandQueue.h"

#include <queue>

class Kernel;

class LODManagerGPU
{
private:
  CommandQueue queue;

  Kernel &scanKernel;
  Kernel &lodPassKernel;
  Kernel &lodPartitionKernel;

  std::queue<cl_event> events;

public:
};