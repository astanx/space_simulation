#pragma once

#include "resources/entity/entity.h"

#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include <cmath>

// Private functions
template <typename Real>
Real EnergyValidator<Real>::calculateError(Real current, Real prev)
{
  if (prev == 0)
    return 0;

  Real error = (current - prev) / prev;
  if (!std::isfinite(error))
    return 0;

  return error;
}

template <typename Real>
void EnergyValidator<Real>::calculateEnergy(const PhysicsDatabaseView<Real> &database, double elapsedTime)
{
  if (this->isFinished())
    return;

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

      Real kineticEnergy = 0.5 * database.getMu(entity) / static_cast<Real>(G) * glm::dot(vec3(database.getVelocity(entity)), vec3(database.getVelocity(entity)));

      quat q = database.getOrientation(entity);

      mat3 R = glm::mat3_cast(q);
      vec3 omega = glm::transpose(R) * database.getAngularVelocity(entity);
      mat3 tensor = database.getInertiaTensor(entity);

      Real rotationalEnergy = 0.5 * glm::dot(omega, tensor * omega);

      for (size_t j = i + 1; j < entities.size(); j++)
      {
        const Entity &otherEntity = entities[j];
        localSystemSample.potentialEnergy -= database.getMu(entity) * database.getMu(otherEntity) / static_cast<Real>(G) / glm::length(vec3(database.getPosition(entity)) - vec3(database.getPosition(otherEntity)));
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

  if (this->historyIdx < this->steps)
    this->historyIdx++;
}
template <typename Real>
void EnergyValidator<Real>::initIndices(const std::vector<Entity> &entities)
{
  for (size_t i = 0; i < entities.size(); i++)
    this->indexToEntity[i] = Entity{entities[i]};
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

  this->initIndices(scene.getSimulationWorld<Real>().getEntityManager().getEntities());
}
template <typename Real>
void EnergyValidator<Real>::update(Scene &scene, double elapsedTime)
{
  SimulationWorld<Real> &world = scene.getSimulationWorld<Real>();
  this->calculateEnergy(world.getPhysicsWorldView(), elapsedTime);
}
template <typename Real>
void EnergyValidator<Real>::sendTable()
{
  if (this->historyIdx == 0)
    return;

  std::cout << std::scientific << std::setprecision(20);

  std::cout << std::endl;
  std::cout << "|      Time      |      Total Energy      |    Potential Energy    |   Total Error    |    Potential Error  |" << std::endl;
  std::cout << "|----------------|------------------------|------------------------|------------------|---------------------|" << std::endl;

  for (size_t i = 0; i < this->historyIdx; ++i)
  {
    SystemSample<Real> &s = this->systemHistory[i];
    std::cout << "| " << s.elapsedTime
              << " | " << s.totalEnergy
              << " | " << s.potentialEnergy
              << " | " << s.totalEnergyError
              << " | " << s.potentialEnergyError
              << " |" << std::endl;
  }
}

template <typename Real>
void EnergyValidator<Real>::saveTable(Scene &scene, const std::filesystem::path &folderPath)
{
  std::filesystem::path systemPath = folderPath / "system_data_history.csv";
  std::ofstream systemFile(systemPath);
  if (!systemFile.is_open())
    Logger::logFatal("Energy Validator", "Couldnt open system history data file");

  systemFile << std::scientific << std::setprecision(std::numeric_limits<Real>::max_digits10);
  systemFile << "time;total_energy;potential_energy;total_energy_error;potential_energy_error" << std::endl;

  for (size_t i = 0; i < this->historyIdx; ++i)
  {
    SystemSample<Real> &s = this->systemHistory[i];
    systemFile << s.elapsedTime << ';'
               << s.totalEnergy << ';'
               << s.potentialEnergy << ';'
               << s.totalEnergyError << ';'
               << s.potentialEnergyError << std::endl;
  }

  systemFile.close();

  const EntityManager &manager = scene.getSimulationWorld<Real>().getEntityManager();

  std::filesystem::path bodyPath = folderPath / "body_data_history.csv";
  std::ofstream bodyFile(bodyPath);
  if (!bodyFile.is_open())
    Logger::logFatal("Energy Validator", "Couldnt open body history data file");

  bodyFile << std::scientific << std::setprecision(std::numeric_limits<Real>::max_digits10);
  bodyFile << "time;id;name;kinetic_energy;rotational_energy;kinetic_error;rotational_error" << std::endl;

  for (size_t i = 0; i < this->historyIdx; i++)
  {
    std::vector<BodySample<Real>> &bodies = this->bodyHistory[i];
    for (size_t j = 0; j < bodies.size(); j++)
    {
      BodySample<Real> &b = bodies[j];
      bodyFile << b.elapsedTime << ';'
               << j << ';'
               << manager.getEntityName(this->indexToEntity.at(j)) << ';'
               << b.kineticEnergy << ';'
               << b.rotationalEnergy << ';'
               << b.kineticEnergyError << ';'
               << b.rotationalEnergyError << std::endl;
    }
  }

  bodyFile.close();

  std::cout << "Body data saved to: " << bodyPath << std::endl;
  std::cout << "System data saved to: " << systemPath << std::endl;
}
