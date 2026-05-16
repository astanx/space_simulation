#pragma once

#include "render/renderable.h"
#include "render/updatable.h"

class ModelSource : public Renderable, public Updatable
{
protected:
  std::unique_ptr<Model> mainLayer;
  std::vector<std::unique_ptr<Model>> layers;
  glm::dvec3 renderPosition;
  const PositionSource *src;
  double radius;

  void updateRenderPosition(glm::dvec3 realPosition, glm::dvec3 camPos);

public:
  ModelSource(const PositionSource *src, double radius);
  virtual ~ModelSource() = default;

  virtual void addMainLayer(std::unique_ptr<Model> m) { this->mainLayer = std::move(m); }
  virtual void addLayer(std::unique_ptr<Model> m) { layers.push_back(std::move(m)); }

  virtual void update(double dt, FrameContext &ctx, Frustum *frustum = nullptr, bool force = false) override;
  virtual void render(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  virtual void renderInstanced(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;

  const glm::dvec3 getRenderPosition() const { return this->renderPosition; };

  static glm::dvec3 realToVisualPos(glm::dvec3 pos, glm::dvec3 camPos = glm::dvec3(0.0));
};