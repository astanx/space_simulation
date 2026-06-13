#include "physics/integrators/wisdomHolman.h"

#include "physics/integrators/integratable.h"

#include "physics/systems/system.h"

#include "physics/object.h"
#include "physics/orbitalObject.h"
#include "physics/constants.h"

#include "render/modelSource.h"

#include "maths/orbitalMaths.h"

// Private functions
void WisdomHolmanIntegrator::halfKickLinear(Object *object, const std::vector<Object *> &objects, double dt)
{
  object->setAcceleration(glm::dvec3(0.0));
  OrbitalObject *orbital = dynamic_cast<OrbitalObject *>(object);
  Object *central = orbital ? orbital->getOrbit()->getCentralBody() : nullptr;

  for (Object *other : objects)
  {
    if (other == object)
      continue;
    if (other == central)
      continue;

    object->applyGravitation(*other);
  }

  object->setVelocity(object->getVelocity() + dt * object->getAcceleration()); // kick
}
void WisdomHolmanIntegrator::halfKickAngular(Object *object, const std::vector<Object *> &objects, double dt)
{
  glm::dvec3 torque = MomentsMaths::calculateTorque(object, objects);
  glm::dvec3 omega = object->getAngularVelocity();
  glm::dmat3 tensor = object->getInertiaProperties().getInertiaTensor();

  glm::dvec3 acc = (torque - glm::cross(omega, (tensor * omega))) / tensor;

  object->setAngularVelocity(object->getAngularVelocity() + dt * acc);
}

void WisdomHolmanIntegrator::keplerDrift(OrbitalObject *object, double dt)
{
  Orbit *orbit = object->getOrbit();
  KeplerElements keplerElements = orbit->getKeplerElements();
  Object *centralBody = orbit->getCentralBody();
  keplerElements.advanceMeanAnomaly(dt);

  double E = OrbitalMaths::calculateEccentricAnomaly(keplerElements.m, keplerElements.e);

  glm::dvec3 pos(0.0);

  pos.x = keplerElements.a * (cos(E) - keplerElements.e);
  pos.y = keplerElements.a * sqrt(1 - (keplerElements.e * keplerElements.e)) * sin(E);

  glm::dvec3 v(0.0);
  double r = keplerElements.a * (1 - keplerElements.e * cos(E));

  v.x = -sqrt(centralBody->getMu() * keplerElements.a) / r * sin(E);
  v.y = sqrt(centralBody->getMu() * keplerElements.a * (1 - (keplerElements.e * keplerElements.e))) / r * cos(E);

  glm::dmat3 R = OrbitalMaths::createR3matrix(keplerElements.Omega) * OrbitalMaths::createR1matrix(keplerElements.i) * OrbitalMaths::createR3matrix(keplerElements.omega);

  object->setVelocity(R * v + centralBody->getVelocity());
  object->setPosition(R * pos + centralBody->getPosition());
  orbit->updateKeplerElements(keplerElements);
}

void WisdomHolmanIntegrator::driftLinear(Object *object, double dt)
{
  OrbitalObject *orbital = dynamic_cast<OrbitalObject *>(object);
  if (orbital)
    this->keplerDrift(orbital, dt);
  else
    object->setPosition(object->getPosition() + object->getVelocity() * dt);
}
void WisdomHolmanIntegrator::driftAngular(Object *object, double dt)
{
  glm::dvec3 omega = object->getAngularVelocity();

  double theta = glm::length(omega) * dt;
  if (theta > EPS)
  {
    glm::dmat3 skew = glm::dmat3(
        glm::dvec3(0, omega.z, -omega.y),
        glm::dvec3(-omega.z, 0, omega.x),
        glm::dvec3(omega.y, -omega.x, 0));

    skew *= dt;

    glm::dmat3 exp = glm::dmat3(1.f) + sin(theta) * skew / theta + (1 - cos(theta)) / theta / theta * skew * skew;

    object->setOrientation(exp * object->getOrientation());
  }
}

void WisdomHolmanIntegrator::drift(Object *object, double dt)
{
  this->driftLinear(object, dt);
  this->driftAngular(object, dt);
}
void WisdomHolmanIntegrator::halfKick(Object *object, const std::vector<Object *> &objects, double dt)
{
  this->halfKickLinear(object, objects, dt);
  this->halfKickAngular(object, objects, dt);
}

// Public functions
void WisdomHolmanIntegrator::step(std::vector<Integratable *> &objects, double dt)
{
  std::vector<Object *> objectPointers;
  std::vector<System *> systemPointers;

  for (Integratable *object : objects)
  {
    if (object->getIsSystem())
    {
      System *sys = dynamic_cast<System *>(object);
      if (sys)
        systemPointers.push_back(sys);
    }
    else
    {
      Object *obj = dynamic_cast<Object *>(object);
      if (obj)
        objectPointers.push_back(obj);
    }
  }

  // kick
  for (Object *object : objectPointers)
    this->halfKick(object, objectPointers, dt * 0.5);
  for (System *system : systemPointers)
    system->forEachObject([this, dt, &objectPointers](Object &object)
                          { this->halfKick(&object, objectPointers, dt * 0.5); });

  // drift
  for (Object *object : objectPointers)
    this->drift(object, dt);
  for (System *system : systemPointers)
    system->forEachObject([this, dt](Object &object)
                          { this->drift(&object, dt); });

  // kick
  for (Object *object : objectPointers)
    this->halfKick(object, objectPointers, dt * 0.5);
  for (System *system : systemPointers)
    system->forEachObject([this, dt, &objectPointers](Object &object)
                          { this->halfKick(&object, objectPointers, dt * 0.5); });
}