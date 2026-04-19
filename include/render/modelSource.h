#pragma once

#include "render/renderable.h"
#include "render/updatable.h"

class ModelSource : public Renderable, public Updatable
{
protected:
  std::unique_ptr<Model> model;
  glm::dvec3 renderPosition;
  const PositionSource *src;

  void updateRenderPosition(glm::dvec3 realPosition);

public:
  ModelSource(const PositionSource *src);
  virtual ~ModelSource() = default;

  virtual void addModel(std::unique_ptr<Model> m) { model = std::move(m); }
  Model *getModel() const { return model.get(); }

  virtual void update(double dt) override;
  virtual void render(Shader &shader) const override;
  virtual void renderInstanced(Shader &shader) const override;

  const glm::dvec3 getRenderPosition() const { return this->renderPosition; };

  static glm::dvec3 realToVisualPos(glm::dvec3 pos);
};