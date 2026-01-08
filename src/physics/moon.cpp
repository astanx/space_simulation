#include "physics/moon.h"
#include "physics/constants.h"
#include "graphics/vertex.h"

// Private functions
void Moon::generateTrail()
{
  std::vector<glm::dvec3> trailVec;
  for (double nu = 0; nu < 2 * M_PI; nu += 0.01)
  {
    glm::dvec3 pos = this->orbitalToInertial(nu);
    pos += this->orbit->getCentralBody()->getPosition();
    pos = this->realToVisualPos(pos);

    trailVec.push_back(pos);
  }

  std::vector<Vertex> vertices;
  for (auto &p : trailVec)
  {
    Vertex v{};
    v.position = p;
    v.color = glm::vec3(1.f);
    v.texcoord = glm::vec2(0.0f);
    v.normal = glm::vec3(0.0f);

    vertices.push_back(v);
  }
  this->trail = std::make_unique<Mesh>(vertices.data(), vertices.size(), nullptr, 0, GL_LINE_LOOP);
}

// Constructor
Moon::Moon(OrbitalObject *centralBody, double mu, double radius, const KeplerElements &keplerElements) : OrbitalObject(centralBody, mu, radius, keplerElements)
{
  if (this->useTrail)
    this->generateTrail();
}

// Public functions
void Moon::update(double dt)
{
  this->move(dt);
  this->model->setPosition(this->renderPosition);
}

void Moon::render(Shader *shader)
{
  if (this->orbit && this->trail && this->useTrail)
    this->renderTrail(shader);

  if (model)
    model->render(shader);
}

void Moon::addModel(std::unique_ptr<Model> model)
{
  this->model = std::move(model);
  this->model->setPosition(this->position * VISUAL_SCALE);
};
