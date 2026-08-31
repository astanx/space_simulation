#pragma once

#include "graphics/model.h"

#include "render/renderPositionSource.h"

#include "render/queue/renderBatch.h"

#include <memory>

class Texture;
class Framebuffer;
class RenderBuffer;
class Camera;

class ReflectanceAcceptor : public Model, public RenderPositionSource
{
private:
  Model *reflector;

  uint radianceSize = 32;

  std::unique_ptr<Texture> radianceTexture;
  std::unique_ptr<Framebuffer> radianceFBO;

  std::unique_ptr<RenderBuffer> RBO;

  void initRadianceTexture();
  void initRadianceFBO();

public:
  ReflectanceAcceptor(Material &material, Mesh &mesh, ModelFlags flags = ModelFlags::None, Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr);
  ReflectanceAcceptor(Mesh &mesh, ModelFlags flags = ModelFlags::None);
  ReflectanceAcceptor(const Model &model);
  // OBJ consturctor
  ReflectanceAcceptor(Material &material, const std::string &OBJfile, ModelFlags flags = ModelFlags::None, Texture *overrideTextureDiffuse = nullptr, Texture *overrideTextureSpecular = nullptr);
  ~ReflectanceAcceptor();

  void setReflector(Model *reflector) { this->reflector = reflector; };
  const Model *getReflector() const { return this->reflector; };

  void render(Shader &shader) const override;
  void renderInstanced(Shader &shader, Buffer *instanceVBO = nullptr, size_t size = 0, size_t count = 0, size_t offset = 0) const override;
  void renderRadiance(Shader &shader, const Camera &camera, Model *reflector) const;
  void renderRadianceInstanced(Shader &shader, const Camera &camera, RenderBatch reflector, Buffer *instanceVBO = nullptr) const;
};