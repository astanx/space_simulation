#pragma once

#include "render/renderer/backend/rendererBackend.h"

class RendererBackendCPU : public RendererBackend
{
private:
public:
  RendererBackendCPU();
  ~RendererBackendCPU();

  void update(Scene &scene, RenderContext &ctx) override;
};