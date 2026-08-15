#include "render/reflectanceAcceptor.h"

#include "debug/logger.h"

#include "camera/camera.h"

#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/buffers/renderBuffer.h"
#include "graphics/framebuffers/framebuffer.h"

#include "graphics/instanceStructs.h"

#include "graphics/bindings/texture.h"

#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedBlending.h"
#include "graphics/state/scopedViewport.h"
#include "graphics/state/scopedDepthTest.h"

// Private functions
void ReflectanceAcceptor::initRadianceTexture()
{
  if (this->radianceTexture)
    return;

  this->radianceTexture = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);

  {
    ScopedTexture radiance(*this->radianceTexture);

    for (unsigned i = 0; i < 6; i++)
      GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F,
                           this->radianceSize, this->radianceSize, 0, GL_RGB, GL_FLOAT, NULL));

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
}

void ReflectanceAcceptor::initRadianceFBO()
{
  if (this->radianceFBO)
    return;

  this->radianceFBO = std::make_unique<Framebuffer>();

  {
    ScopedFramebuffer fbo(*this->radianceFBO, GL_FRAMEBUFFER);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        this->radianceTexture->getId(),
        0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    this->RBO = std::make_unique<RenderBuffer>();

    this->RBO->bind(GL_RENDERBUFFER);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                          this->radianceSize, this->radianceSize);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, this->RBO->getId());

    this->radianceFBO->checkComplete();
  }
}

// Constructors / Destructor
ReflectanceAcceptor::ReflectanceAcceptor(Material &material, Mesh &mesh, ModelFlags flags, Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular) : Model(material, mesh, flags, overrideTextureDiffuse, overrideTextureSpecular)
{
  this->initRadianceTexture();
  this->initRadianceFBO();
}
ReflectanceAcceptor::ReflectanceAcceptor(Mesh &mesh, ModelFlags flags) : Model(mesh, flags)
{
  this->initRadianceTexture();
  this->initRadianceFBO();
}
ReflectanceAcceptor::ReflectanceAcceptor(const Model &model) : Model(model)
{
  this->initRadianceTexture();
  this->initRadianceFBO();
}
// OBJ consturctor
ReflectanceAcceptor::ReflectanceAcceptor(Material &material, const std::string &OBJfile, ModelFlags flags, Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular) : Model(material, OBJfile, flags, overrideTextureDiffuse, overrideTextureSpecular)
{
  this->initRadianceTexture();
  this->initRadianceFBO();
}

ReflectanceAcceptor::~ReflectanceAcceptor() = default;

void ReflectanceAcceptor::render(Shader &shader)
{
  ScopedTexture radianceTextureScope(*this->radianceTexture, TextureBindingPoints::EnvironmentMap);
  //fix
  // glm::vec3 reflectorPosition = reflectorPosition;
  glm::vec3 reflectorPosition(0.0);
  shader.set1i(1, "useReflectorRadiance");
  shader.setVec3f(reflectorPosition, "reflectorPosition");
  shader.set1i(TextureBindingPoints::EnvironmentMap, "reflectorRadianceCubemap");

  Model::render(shader);

  shader.set1i(0, "useReflectorRadiance");
}
void ReflectanceAcceptor::renderInstanced(Shader &shader, Buffer *instanceVBO, size_t size, size_t count, size_t offset)
{
  Model::renderInstanced(shader, instanceVBO, size, count, offset);
}

void ReflectanceAcceptor::renderRadiance(Shader &shader, const Camera &camera, Model *reflector) const
{
  // glm::vec3 pos = acceptorPosition;
  glm::vec3 pos(0.0);

  shader.setVec3f(pos, "receiverPosition");

  glm::mat4 projection = camera.getProjectionMatrix(1, 90.0f);

  std::vector<glm::mat4> views = {
      glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
      glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
      glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
      glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
      glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
      glm::lookAt(pos, pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))};

  {
    ScopedFramebuffer fbo(*this->radianceFBO, GL_FRAMEBUFFER);
    ScopedViewport viewport(0, 0, this->radianceSize, this->radianceSize);
    ScopedDepthTest depthTest(true);
    ScopedBlending blend(false);

    for (int i = 0; i < 6; i++)
    {
      GL_CALL(glFramebufferTexture2D(
          GL_FRAMEBUFFER,
          GL_COLOR_ATTACHMENT0,
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          this->radianceTexture->getId(),
          0));

      glClearColor(0.f, 0.f, 0.f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      shader.setMat4fv(projection, "ProjectionMatrix");
      shader.setMat4fv(views[i], "ViewMatrix");

      reflector->render(shader);
    }
  }
}

void ReflectanceAcceptor::renderRadianceInstanced(Shader &shader, const Camera &camera, RenderBatch reflector, Buffer *instanceVBO) const
{
  //fix
  // glm::vec3 pos = acceptorPosition;
  glm::vec3 pos(0.0);

  shader.setVec3f(pos, "receiverPosition");

  glm::mat4 projection = camera.getProjectionMatrix(1, 90.0f);

  std::vector<glm::mat4> views = {
      glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)),
      glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)),
      glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
      glm::lookAt(pos, pos + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)),
      glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)),
      glm::lookAt(pos, pos + glm::vec3(0, 0, -1), glm::vec3(0, -1, 0))};

  {
    ScopedFramebuffer fbo(*this->radianceFBO, GL_FRAMEBUFFER);
    ScopedViewport viewport(0, 0, this->radianceSize, this->radianceSize);
    ScopedDepthTest depthTest(true);
    ScopedBlending blend(false);

    for (int i = 0; i < 6; i++)
    {
      GL_CALL(glFramebufferTexture2D(
          GL_FRAMEBUFFER,
          GL_COLOR_ATTACHMENT0,
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          this->radianceTexture->getId(),
          0));

      glClearColor(0.f, 0.f, 0.f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      shader.setMat4fv(projection, "ProjectionMatrix");
      shader.setMat4fv(views[i], "ViewMatrix");

      size_t size = sizeof(InstanceModelMatrixParts);

      reflector.model->renderInstanced(shader, instanceVBO, size, reflector.range.end - reflector.range.begin, reflector.range.begin);
    }
  }
}