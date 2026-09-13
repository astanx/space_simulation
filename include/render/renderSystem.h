#pragma once

class RenderQueue;
class LODManager;
class InstanceManager;
class Camera;
class Texture;
class Model;
struct Frustum;

class RenderSystem
{
protected:
  std::vector<Model *> models;

public:
  std::vector<Model *> &getModels() { return this->models; };
  virtual Model *getModelFromObjectIndex(size_t i) = 0;
};