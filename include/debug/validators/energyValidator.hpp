#pragma once

#include "resources/entity/entity.h"

#include <glm/glm.hpp>

// Private functions
template <typename Real>
Real EnergyValidator<Real>::calculateError(Real current, Real prev)
{
  Real error = (current - prev) / prev;
  if (!std::isfinite(error))
    return 0;

  return error;
}

template <typename Real>
void EnergyValidator<Real>::calculateEnergy(const PhysicsDatabaseView<Real> &database, double elapsedTime)
{
  if (this->historyIdx >= this->steps - 1)

  {
    std::cout << "TOTAL 0: " << std::setprecision(30) << this->calculateError(this->systemHistory[this->historyIdx - 1].totalEnergy, this->systemHistory[0].totalEnergy) << std::endl;
    std::cout << "POTENTIAL 0: " << std::setprecision(30) << this->calculateError(this->systemHistory[this->historyIdx - 1].potentialEnergy, this->systemHistory[0].potentialEnergy) << std::endl;
    std::cout << "ROTATIONAL 0: " << std::setprecision(30) << this->calculateError(this->bodyHistory[this->historyIdx - 1][3].rotationalEnergy, this->bodyHistory[0][3].rotationalEnergy) << std::endl;
    std::cout << "KINETIC 0: " << std::setprecision(30) << this->calculateError(this->bodyHistory[this->historyIdx - 1][3].kineticEnergy, this->bodyHistory[0][3].kineticEnergy) << std::endl;
    return;
  }

  const std::vector<Entity> &entities = database.getEntities();

  this->bodyHistory[this->historyIdx].resize(entities.size());
  this->systemHistory[this->historyIdx] = SystemSample<Real>{};

  std::vector<SystemSample<Real>> tempSystemSamples(this->threadPool.getThreadCount());

  this->threadPool.parallelFor(0, entities.size(), [this, &database, &entities, &tempSystemSamples, elapsedTime](Range work, size_t threadIdx)
                               {
    SystemSample<Real> &localSystemSample = tempSystemSamples[threadIdx];
    for (size_t i = work.begin; i < work.end; i++)
    {
      const Entity &entity = entities[i];

      Real kineticEnergy = 0.5 * database.getMu(entity) / G * glm::dot(vec3(database.getVelocity(entity)), vec3(database.getVelocity(entity)));

      quat q = database.getOrientation(entity);

      mat3 R = glm::mat3_cast(q);
      vec3 omega = glm::transpose(R) * database.getAngularVelocity(entity);
      mat3 tensor = database.getInertiaTensor(entity);

      Real rotationalEnergy = 0.5 * glm::dot(omega, tensor * omega);

      for (size_t j = i + 1; j < entities.size(); j++)
      {
        const Entity &otherEntity = entities[j];
        localSystemSample.potentialEnergy -= database.getMu(entity) * database.getMu(otherEntity) / G  / glm::length(vec3(database.getPosition(entity)) - vec3(database.getPosition(otherEntity)));
      }
      localSystemSample.totalEnergy += kineticEnergy + rotationalEnergy;

      Real prevKinetic = kineticEnergy;
      Real prevRotational = rotationalEnergy;

      if (this->historyIdx > 0)
      {
        BodySample<Real> prevBody = this->bodyHistory[this->historyIdx - 1][i];
        prevKinetic = prevBody.kineticEnergy;
        prevRotational = prevBody.rotationalEnergy;
      }

      if (i == 3)
      {
  std::cout << std::setprecision(30) << "KINETIC ERROR: " << this->calculateError(kineticEnergy, prevKinetic) << std::endl;
  std::cout << std::setprecision(30) << "ROTATIONAL ERROR: " <<  this->calculateError(rotationalEnergy, prevRotational) << std::endl;
  std::cout << std::endl;
      }


      this->bodyHistory[this->historyIdx][i] = BodySample<Real>{
        static_cast<Real>(elapsedTime), 
        kineticEnergy, rotationalEnergy,
        this->calculateError(kineticEnergy, prevKinetic),
        this->calculateError(rotationalEnergy, prevRotational)
      };
    } 
    localSystemSample.totalEnergy += localSystemSample.potentialEnergy; });

  for (SystemSample<Real> &local : tempSystemSamples)
  {
    this->systemHistory[this->historyIdx].totalEnergy += local.totalEnergy;
    this->systemHistory[this->historyIdx].potentialEnergy += local.potentialEnergy;
  }

  this->systemHistory[this->historyIdx].elapsedTime = elapsedTime;

  Real prevPotential = this->systemHistory[this->historyIdx].potentialEnergy;
  Real prevTotal = this->systemHistory[this->historyIdx].totalEnergy;

  if (this->historyIdx > 0)
  {
    SystemSample<Real> prevSys = this->systemHistory[this->historyIdx - 1];
    prevPotential = prevSys.potentialEnergy;
    prevTotal = prevSys.totalEnergy;
  }

  this->systemHistory[this->historyIdx].totalEnergyError = this->calculateError(this->systemHistory[this->historyIdx].totalEnergy, prevTotal);
  this->systemHistory[this->historyIdx].potentialEnergyError = this->calculateError(this->systemHistory[this->historyIdx].potentialEnergy, prevPotential);

  std::cout << std::setprecision(30) << "POTENTIAL ERROR: " << this->systemHistory[this->historyIdx].potentialEnergyError << std::endl;
  std::cout << std::setprecision(30) << "TOTAL ERROR: " << this->systemHistory[this->historyIdx].totalEnergyError << std::endl;
  std::cout << std::endl;

  if (this->historyIdx + 1 < this->steps)
    this->historyIdx++;
}

// Constructor / Destructor
template <typename Real>
EnergyValidator<Real>::EnergyValidator(ThreadPool &threadPool) : threadPool(threadPool) {};
template <typename Real>
EnergyValidator<Real>::~EnergyValidator() = default;

// Public functions
template <typename Real>
void EnergyValidator<Real>::init(Scene &scene, double elapsedTime, size_t steps)
{
  this->steps = steps;
  this->bodyHistory.resize(this->steps);
  this->systemHistory.resize(this->steps);

  this->update(scene, elapsedTime);
}
template <typename Real>
void EnergyValidator<Real>::update(Scene &scene, double elapsedTime)
{
  SimulationWorld<Real> &world = scene.getSimulationWorld<Real>();
  this->calculateEnergy(world.getPhysicsWorldView(), elapsedTime);
}
