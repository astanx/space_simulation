#pragma once

#include "render/renderer/backend/rendererBackend.h"

class RendererBackendGPU : public RendererBackend
{
private:
public:
  RendererBackendGPU();
  ~RendererBackendGPU();

  void update(Scene &scene, RenderContext &ctx) override;
};