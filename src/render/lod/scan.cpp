#include "render/lod/manager/scan.h"

#include "compute/commandQueue.h"

#include "resources/resourceManager.h"

// Constructor
Scan::Scan(ResourceManager &manager, const std::string &localScanName, const std::string &groupScanName, const std::string &groupOffsetScanName) : localScanKernel(manager.GetKernel(localScanName)),
                                                                                                                                                   groupScanKernel(manager.GetKernel(groupScanName)),
                                                                                                                                                   groupOffsetScanKernel(manager.GetKernel(groupOffsetScanName)) {};

// Public functions
void Scan::initBuffers(cl_context ctx, CommandQueue &queue, size_t totalObjects, size_t groupCount)
{
  this->isBuffer.init(ctx, CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->offsetBuffer.init(ctx, CL_MEM_READ_WRITE, totalObjects * sizeof(uint32_t), nullptr);
  this->groupSumsBuffer.init(ctx, CL_MEM_READ_WRITE, groupCount * sizeof(uint32_t), nullptr);
  this->groupOffsetsBuffer.init(ctx, CL_MEM_READ_WRITE, groupCount * sizeof(uint32_t), nullptr);

  std::vector<uint32_t> zeros(totalObjects, 0);
  queue.enqueueWriteBuffer(this->isBuffer.get(), CL_FALSE, 0, totalObjects * sizeof(uint32_t), zeros.data());
  queue.enqueueWriteBuffer(this->offsetBuffer.get(), CL_FALSE, 0, totalObjects * sizeof(uint32_t), zeros.data());

  std::vector<uint32_t> zerosGroup(groupCount, 0);
  queue.enqueueWriteBuffer(this->groupSumsBuffer.get(), CL_FALSE, 0, groupCount * sizeof(uint32_t), zerosGroup.data());
  queue.enqueueWriteBuffer(this->groupOffsetsBuffer.get(), CL_FALSE, 0, groupCount * sizeof(uint32_t), zerosGroup.data());
}
void Scan::initKernels(size_t totalObjects, size_t localScanSize, size_t groupCount)
{
  this->localScanKernel.setArg(0, this->isBuffer.get());
  this->localScanKernel.setArg(1, this->offsetBuffer.get());
  this->localScanKernel.setArg(2, this->groupSumsBuffer.get());
  this->localScanKernel.setArg(3, localScanSize * sizeof(uint32_t), nullptr);
  this->localScanKernel.setArg(4, sizeof(uint32_t), &totalObjects);

  this->groupScanKernel.setArg(0, this->groupSumsBuffer.get());
  this->groupScanKernel.setArg(1, this->groupOffsetsBuffer.get());
  this->groupScanKernel.setArg(2, localScanSize * sizeof(uint32_t), nullptr);
  this->groupScanKernel.setArg(3, sizeof(uint32_t), &groupCount);

  this->groupOffsetScanKernel.setArg(0, this->offsetBuffer.get());
  this->groupOffsetScanKernel.setArg(1, this->groupOffsetsBuffer.get());
  this->groupOffsetScanKernel.setArg(2, sizeof(uint32_t), &totalObjects);
}

void Scan::enqueueKernels(CommandQueue &queue, size_t globalObjects, size_t groupsGlobal, size_t localScanSize)
{
  queue.enqueueNDKernelBuffer(this->localScanKernel.get(), 1, NULL, &globalObjects, &localScanSize);
  queue.enqueueNDKernelBuffer(this->groupScanKernel.get(), 1, NULL, &groupsGlobal, &localScanSize);
  queue.enqueueNDKernelBuffer(this->groupOffsetScanKernel.get(), 1, NULL, &globalObjects, &localScanSize);
}