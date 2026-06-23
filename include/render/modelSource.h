#pragma once

#include "render/renderable.h"
#include "render/updatable.h"

class ModelSource : public Renderable, public Updatable
{
protected:
  std::unique_ptr<Model> mainLayer;
  std::vector<std::unique_ptr<Model>> layers;
  glm::dvec3 renderPosition;
  glm::dmat3 renderOrientation;
  const PositionSource &src;
  double radius;

  virtual void renderLayers(Shader &shader) const;

public:
  ModelSource(const PositionSource &src, double radius);
  virtual ~ModelSource() = default;

  virtual void addMainLayer(std::unique_ptr<Model> m) { this->mainLayer = std::move(m); }
  virtual void addLayer(std::unique_ptr<Model> m) { layers.push_back(std::move(m)); }

  virtual void update(const Camera &camera) override;
  virtual void render(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  virtual void renderInstanced(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;

  const glm::dvec3 getRenderPosition() const { return this->renderPosition; };
};