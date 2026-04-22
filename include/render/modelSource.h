#pragma once

#include "render/renderable.h"
#include "render/updatable.h"

class ModelSource : public Renderable, public Updatable
{
protected:
  std::unique_ptr<Model> model;
  glm::dvec3 renderPosition;
  const PositionSource *src;
  double radius;

  void updateRenderPosition(glm::dvec3 realPosition);

public:
  ModelSource(const PositionSource *src, double radius);
  virtual ~ModelSource() = default;

  virtual void addModel(std::unique_ptr<Model> m) { model = std::move(m); }
  Model *getModel() const { return model.get(); }

  virtual void update(double dt, Frustum* frustum = nullptr, bool force = false) override;
  virtual void render(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  virtual void renderInstanced(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;

  const glm::dvec3 getRenderPosition() const { return this->renderPosition; };

  static glm::dvec3 realToVisualPos(glm::dvec3 pos);
};