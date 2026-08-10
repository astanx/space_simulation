#pragma once

#include "compute/clBuffer.h"

#include <string>

class Kernel;
class ResourceManager;
class CommandQueue;

class Scan
{
private:
  CLBuffer isBuffer;
  CLBuffer offsetBuffer;
  CLBuffer groupSumsBuffer;
  CLBuffer groupOffsetsBuffer;
  Kernel &localScanKernel;
  Kernel &groupScanKernel;
  Kernel &groupOffsetScanKernel;

public:
  Scan(ResourceManager &manager, const std::string &localScanName, const std::string &groupScanName, const std::string &groupOffsetScanName);
  ~Scan() = default;

  void initKernels(size_t totalObjects, size_t localScanSize, size_t groupCount);
  void initBuffers(cl_context ctx, CommandQueue &queue, size_t totalObjects, size_t groupCount);

  void enqueueKernels(CommandQueue &queue, size_t globalObjects, size_t groupsGlobal, size_t localScanSize);

  CLBuffer& getIsBuffer() { return this->isBuffer; };
  CLBuffer& getOffsetBuffer() { return this->offsetBuffer; };
};