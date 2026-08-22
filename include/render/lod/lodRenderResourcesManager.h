#pragma once

#include <memory>

class Texture;
class Mesh;
class Model;
class ModelSource;
class RenderSystem;

class LODRenderResourcesManager
{
private:
  bool isImpostorInitialized = false;
  std::unique_ptr<Texture> impostorTexture;
  std::unique_ptr<Mesh> impostorMesh;
  std::unique_ptr<Mesh> pointMesh;

  void initImpostor(std::vector<Model *> &models);
  void initPoint();
  void bindLayerToImpostorTexture(Model &model, unsigned int layer);
  void bindLayerToImpostorTexture(Model *model, unsigned int layer);

public:
  LODRenderResourcesManager() = default;
  ~LODRenderResourcesManager() = default;

  void init(std::vector<Model *> &models);

  Mesh &getImpostorMesh() { return *this->impostorMesh.get(); };
  Texture &getImpostorTexture() { return *this->impostorTexture.get(); };
  Mesh &getPointMesh() { return *this->pointMesh.get(); };
};