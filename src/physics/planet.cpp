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

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
    GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->moonRadianceTexture->getId(), 0));

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    this->moonRadianceFBO->checkComplete();
  }
}

// Constructor
Planet::Planet(Object *centralBody, double mu, double radius, const KeplerElements &keplerElements) : OrbitalObject(centralBody, mu, radius, keplerElements)
{
}

// Public functions
void Planet::update(double dt)
{
  // this->move(dt);

  for (std::unique_ptr<Moon> &moon : this->moons)
  {
    moon->update(dt);
  }
  if (this->model)
    this->model->setPosition(this->renderPosition);
}

void Planet::render(Shader &shader) const
{
  for (const std::unique_ptr<Moon> &moon : this->moons)
    moon->render(shader);

  if (this->model)
    this->model->render(shader);
};

void Planet::renderMoonsRadiance(Shader &shader, const Camera &camera) const
{
  if (this->moons.empty())
    return;

  shader.setVec3f(glm::vec3(this->renderPosition), "receiverPosition");

  glm::mat4 projection = camera.getProjectionMatrix(1, 90.0f);

  glm::vec3 pos = this->renderPosition;

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
    ScopedDepthTest depthTest(false);

    double moonsRadiance = 0.0;

    for (int i = 0; i < 6; i++)
    {

      GL_CALL(glFramebufferTexture2D(
          GL_FRAMEBUFFER,
          GL_COLOR_ATTACHMENT0,
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          moonRadianceTexture->getId(),
          0));

      glClear(GL_COLOR_BUFFER_BIT);

      shader.setMat4fv(projection, "ProjectionMatrix");
      shader.setMat4fv(views[i], "ViewMatrix");

      for (const auto &moon : moons)
      {
        moon->sendHapkeParametersToShader(shader);
        moon->render(shader);
      }

      std::vector<float> pixels(this->radianceSize * this->radianceSize * 3);
      glReadPixels(0, 0, this->radianceSize, this->radianceSize, GL_RGB, GL_FLOAT, pixels.data());

      for (auto &pixel : pixels)
        moonsRadiance += pixel;
    }

    std::cout << "Moons radiance: " << moonsRadiance / (this->radianceSize * this->radianceSize * 6) << std::endl;
  }
}

void Planet::addModel(std::unique_ptr<Model> model)
{
  this->model = std::move(model);
  this->model->setPosition(this->renderPosition);
};

void Planet::addMoon(std::unique_ptr<Moon> moon)
{
  this->moons.push_back(std::move(moon));

  this->initMoonRadianceTexture();
  this->initMoonRadianceFBO();
}

void Planet::drift(double dt)
{
  this->keplerDrift(dt);

  for (std::unique_ptr<Moon> &moon : this->moons)
    moon->drift(dt);
}

void Planet::halfKick(const std::vector<Object *> &bodies, double dt)
{
  this->kick(bodies, dt * 0.5);

  for (std::unique_ptr<Moon> &moon : this->moons)
    moon->halfKick(bodies, dt);
}