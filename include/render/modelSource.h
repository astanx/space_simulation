#pragma once

#include "render/renderable.h"
#include "render/updatable.h"
#include "render/renderFlags.h"

#include "graphics/materials/material.h"

#include "graphics/model.h"

#include "physics/transformSource.h"

#include <functional>

class ModelSource : public Renderable, public Updatable
{
protected:
  std::unique_ptr<Model> mainLayer;
  std::vector<std::unique_ptr<Model>> layers;
  glm::mat4 modelMatrix;
  glm::vec3 renderPosition;
  glm::vec3 renderScale;
  glm::quat renderOrientation;
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

  virtual void update(const Camera &camera) override;
  virtual void render(Shader &shader) override;
  virtual void renderLayers(Shader &shader) const;
  virtual void renderLayersInstanced(Shader &shader) const;
  virtual void renderInstanced(Shader &shade, Buffer *instanceVBO = nullptr, size_t size = 0, size_t count = 0, size_t offset = 0) override;

  template <typename F>
  void forEachModel(F &&func);

  const glm::vec3 getRenderPosition() const { return this->renderPosition; };
  const glm::quat getRenderOrientation() const { return this->renderOrientation; };
  const double getRenderRadius() const { return this->renderRadius; };
  const float getRenderImportance() const { return this->renderImportance; };
  const Radii getSrcRadii() const { return this->src.getRadii(); };
  const double getWorldRadius() const { return this->src.getRadius(); };
  const Model *getMainLayer() const { return this->mainLayer.get(); };
  const Texture *getMainLayerTexture() const { return this->mainLayer->getMaterial()->getTexture(); };
};

#include "render/modelSource.tpp"