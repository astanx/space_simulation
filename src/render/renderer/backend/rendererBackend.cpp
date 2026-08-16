#include "render/renderer/backend/rendererBackend.h"

#include "render/renderContext.h"

#include "scene/scene.h"
#include "scene/light/pointLight.h"

#include "resources/resourceManager.h"

#include "camera/cameraGPU.h"
#include "camera/camera.h"

#include "graphics/shader.h"
#include "graphics/bindings/ubo.h"
#include "graphics/bindings/texture.h"
#include "graphics/state/scopedBuffer.h"

// Private functions
void RendererBackend::initShaderBuffer(Buffer &ubo, unsigned long size, GLenum bufferType)
{
  ScopedBuffer buff(ubo, GL_UNIFORM_BUFFER);
  GL_CALL(glBufferData(bufferType, size, nullptr, GL_DYNAMIC_DRAW));
}
void RendererBackend::initShaderUBOBindings(ResourceManager &manager)
{
  std::vector<Shader *> shaders = manager.GetAllShaders();

  for (Shader *shader : shaders)
  {
    if (!shader)
      continue;

    GLuint programID = shader->getId();

    GLuint cameraBlockIndex = glGetUniformBlockIndex(programID, "Camera");
    if (cameraBlockIndex != GL_INVALID_INDEX)
      glUniformBlockBinding(programID, cameraBlockIndex, UBOBindingPoints::Camera);

    this->lightManager.initDirLightUBOBinding(programID);
    this->lightManager.initPointLightUBOBinding(programID);

    this->shadowManager.initDirShadowUBOBinding(programID);
    this->shadowManager.initPointShadowUBOBinding(programID);
  }
}

void RendererBackend::updateCameraUBO(Scene &scene, RenderContext &ctx)
{
  const Camera &activeCamera = scene.getActiveCamera();

  CameraGPU camUBO{};
  camUBO.ProjectionMatrix = activeCamera.getProjectionMatrix(ctx.frameCtx.aspect);
  camUBO.ViewMatrix = activeCamera.getViewMatrix();
  camUBO.camPosition = glm::vec4(0.0);

  ScopedBuffer ubo(this->cameraUBO, GL_UNIFORM_BUFFER);
  GL_CALL(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), &camUBO));
}

// Constructor / Desctructor
RendererBackend::RendererBackend() = default;
RendererBackend::~RendererBackend() = default;

// Public functions
void RendererBackend::init(Scene &scene, ResourceManager &manager, size_t shadowRes)
{
  const PointLight *pointLight = scene.getPointLight();
  if (pointLight)
  {
    this->lightManager.initPointUBO();
    this->shadowManager.initPointUBO();

    const Camera &activeCamera = scene.getActiveCamera();

    this->shadowManager.addPointShadow(std::make_unique<PointShadow>(shadowRes, shadowRes,
                                                                     pointLight->getPosition(),
                                                                     activeCamera.getNearPlane(),
                                                                     activeCamera.getFarPlane()));

    // Multiple-lights(not supported on opengl < 4.2)
    // this->initShaderBuffer(&this->lightManager->getPointSSBO(), sizeof(PointLightGPU) * this->pointLights.size(), GL_SHADER_STORAGE_BUFFER);
  }

  if (scene.getDirLight())
  {
    this->lightManager.initDirUBO();
    this->shadowManager.initDirUBO();
    this->shadowManager.addDirShadow(std::make_unique<DirectionalShadow>(shadowRes, shadowRes));
  }

  this->initShaderBuffer(this->cameraUBO, sizeof(CameraGPU), GL_UNIFORM_BUFFER);
  this->initShaderUBOBindings(manager);
}

void RendererBackend::bindUBOs()
{
  this->cameraUBO.bindBufferBase(GL_UNIFORM_BUFFER, UBOBindingPoints::Camera);

  this->lightManager.bindDirLightUBO();
  this->lightManager.bindPointLightUBO();

  this->shadowManager.bindDirShadowUBO();
  this->shadowManager.bindPointShadowUBO();
}
void RendererBackend::bindDummyReflector(Shader &shader)
{
  glActiveTexture(GL_TEXTURE0 + TextureBindingPoints::EnvironmentMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, TextureBindingPoints::EnvironmentMap);
  shader.set1i(TextureBindingPoints::EnvironmentMap, "reflectorRadianceCubemap");
  shader.set1i(0, "useReflectorRadiance");
}

void RendererBackend::bindPointShadow(Shader &shader, const std::string &name)
{
  this->shadowManager.bindPointShadow(shader, name);
}
void RendererBackend::bindPointShadowDepth(Shader &shader, const std::string &name)
{
  this->shadowManager.bindPointShadowDepth(shader, name);
}
const Framebuffer &RendererBackend::getDirShadowFramebuffer()
{
  return this->shadowManager.getDirShadow()->getShadowFramebuffer();
}
const Framebuffer &RendererBackend::getPointShadowFramebuffer()
{
  return this->shadowManager.getPointShadow()->getShadowFramebuffer();
}
const Texture &RendererBackend::getPointShadowMapTexture()
{
  return this->shadowManager.getPointShadow()->getShadowMapTexture();
}