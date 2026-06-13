#include "physics/planet.h"

#include "debug/logger.h"

#include "camera/camera.h"

#include "physics/object.h"
#include "physics/constants.h"
#include "physics/orbit.h"

#include "graphics/model.h"
#include "graphics/shader.h"
#include "graphics/vertex.h"
#include "graphics/texture.h"

#include "graphics/state/scopedFramebuffer.h"
#include "graphics/state/scopedViewport.h"
#include "graphics/state/scopedDepthTest.h"
#include "graphics/state/scopedTexture.h"
#include "graphics/state/scopedBlending.h"

#include "graphics/bindings/texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// Private functions
void Planet::initMoonRadianceTexture()
{
  if (this->moonRadianceTexture)
    return;

  this->moonRadianceTexture = std::make_unique<Texture>(GL_TEXTURE_CUBE_MAP);

  {
    ScopedTexture moonRadiance(*this->moonRadianceTexture);

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

void Planet::initMoonRadianceFBO()
{
  if (this->moonRadianceFBO)
    return;

  this->moonRadianceFBO = std::make_unique<Framebuffer>();

  {
    ScopedFramebuffer fbo(*this->moonRadianceFBO, GL_FRAMEBUFFER);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        this->moonRadianceTexture->getId(),
        0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    this->moonRBO = std::make_unique<RenderBuffer>();

    this->moonRBO->bind(GL_RENDERBUFFER);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                          this->radianceSize, this->radianceSize);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, this->moonRBO->getId());

    this->moonRadianceFBO->checkComplete();
  }
}

// Constructor / Destructor
Planet::Planet(Object *centralBody, double mu, Radii radii, const KeplerElements &keplerElements)
    : OrbitalObject(centralBody, mu, radii, keplerElements), ModelSource(static_cast<const PositionSource &>(*this), radii.mean * VISUAL_RADIUS_SCALE)
{
}

Planet::~Planet() = default;

// Public functions
void Planet::render(Shader &shader, Frustum *frustum, bool force) const
{
  std::optional<ScopedTexture> moonRadianceTextureScope;

  if (!this->moons.empty())
  {
    moonRadianceTextureScope.emplace(*this->moonRadianceTexture, TextureBindingPoints::EnvironmentMap);
    shader.set1i(1, "useReflectorRadiance");
    shader.setVec3f(this->moons[0]->getRenderPosition(), "reflectorPosition");
    shader.set1i(TextureBindingPoints::EnvironmentMap, "reflectorRadianceCubemap");
  }
  else
    shader.set1i(0, "useReflectorRadiance");

  ModelSource::render(shader, frustum);
};

void Planet::renderMoonsRadiance(Shader &shader, const Camera &camera) const
{
  if (this->moons.empty())
    return;

  glm::vec3 pos = this->renderPosition;

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
    ScopedFramebuffer fbo(*this->moonRadianceFBO, GL_FRAMEBUFFER);
    ScopedViewport viewport(0, 0, this->radianceSize, this->radianceSize);
    ScopedDepthTest depthTest(true);
    ScopedBlending blend(false);

    for (int i = 0; i < 6; i++)
    {
      GL_CALL(glFramebufferTexture2D(
          GL_FRAMEBUFFER,
          GL_COLOR_ATTACHMENT0,
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          moonRadianceTexture->getId(),
          0));

      glClearColor(0.f, 0.f, 0.f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      shader.setMat4fv(projection, "ProjectionMatrix");
      shader.setMat4fv(views[i], "ViewMatrix");

      for (const auto &moon : moons)
      {
        moon->sendHapkeParametersToShader(shader);
        moon->render(shader, nullptr, true);
      }
    }
  }
}

void Planet::addMoon(std::unique_ptr<Moon> moon)
{
  this->moons.push_back(std::move(moon));

  this->initMoonRadianceTexture();
  this->initMoonRadianceFBO();
}