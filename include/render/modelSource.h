#pragma once

#include "physics/transformSource.h"

#include "render/renderable.h"
#include "render/updatable.h"

class ModelSource : public Renderable, public Updatable
{
protected:
  std::unique_ptr<Model> mainLayer;
  std::vector<std::unique_ptr<Model>> layers;
  glm::dvec3 renderPosition;
  glm::dmat3 renderOrientation;
  const TransformSource &src;
  float renderImportance;
  double renderRadius;

public:
  ModelSource(const TransformSource &src, double renderRadius);
  virtual ~ModelSource() = default;

  void scaleRadii(Radii scaledRadii);

  virtual void addMainLayer(std::unique_ptr<Model> m) { this->mainLayer = std::move(m); };
  virtual void addLayer(std::unique_ptr<Model> m) { layers.push_back(std::move(m)); };
  void setRenderRadius(double radius) { this->renderRadius = radius; };
  void setRenderImportance(float importance) { this->renderImportance = importance; };

  virtual void update(const Camera &camera, Frustum *frustum = nullptr, bool force = false) override;
  virtual void render(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;
  virtual void renderLayers(Shader &shader, Frustum *frustum = nullptr, bool force = false) const;
  virtual void renderInstanced(Shader &shader, Frustum *frustum = nullptr, bool force = false) const override;

  const glm::dvec3 getRenderPosition() const { return this->renderPosition; };
  const double getRenderRadius() const { return this->renderRadius; };
  const float getRenderImportance() const { return this->renderImportance; };
  const Radii getSrcRadii() const { return this->src.getRadii(); };
  const double getWorldRadius() const { return this->src.getRadius(); };
  const Model *getMainLayer() const { return this->mainLayer.get(); };
};