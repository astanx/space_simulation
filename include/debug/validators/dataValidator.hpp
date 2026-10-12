#pragma once

#include "resources/entity/entity.h"
#include "resources/data/gpuTypes.h"

#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include <cmath>

// Private functions
template <typename Real, typename WorldReal>
void DataValidator<Real, WorldReal>::saveData(const PhysicsDatabaseView<WorldReal> &database, Date date)
{
  if (this->isFinished())
    return;

  const std::vector<Entity> &entities = database.getEntities();

  this->bodyHistory[this->historyIdx].resize(entities.size(), DataBodySample<Real>{date});

  this->threadPool.parallelFor(0, entities.size(), [this, &database, &entities](Range work, size_t threadIdx)
                               {
    for (size_t i = work.begin; i < work.end; i++)
    {
      const Entity &entity = entities[i];
      vec3 centralPos = database.getIsOrbital(entity) ? vec3(database.getPosition(database.getCentralBodyIdx(entity))) : vec3(0.0);
      vec3 centralVel = database.getIsOrbital(entity) ? vec3(database.getVelocity(database.getCentralBodyIdx(entity))) : vec3(0.0);
      this->bodyHistory[this->historyIdx][i].relativePosition = vec3(database.getPosition(entity)) - centralPos;
      this->bodyHistory[this->historyIdx][i].relativeVelocity = vec3(database.getVelocity(entity)) - centralVel;
    }; });

  if (this->historyIdx < this->steps)
    this->historyIdx++;
}
template <typename Real, typename WorldReal>
void DataValidator<Real, WorldReal>::initIndices(const std::vector<Entity> &entities)
{
  for (size_t i = 0; i < entities.size(); i++)
    this->indexToEntity[i] = Entity{entities[i]};
}

// Constructor / Destructor
template <typename Real, typename WorldReal>
DataValidator<Real, WorldReal>::DataValidator(ThreadPool &threadPool) : threadPool(threadPool) {};
template <typename Real, typename WorldReal>
DataValidator<Real, WorldReal>::~DataValidator() = default;

// Public functions
template <typename Real, typename WorldReal>
void DataValidator<Real, WorldReal>::init(Scene &scene, Date date, size_t steps)
{
  this->steps = steps;
  this->bodyHistory.resize(this->steps);

  this->initIndices(scene.getSimulationWorld<WorldReal>().getEntityManager().getEntities());
}
template <typename Real, typename WorldReal>
void DataValidator<Real, WorldReal>::update(Scene &scene, Date date)
{
  SimulationWorld<WorldReal> &world = scene.getSimulationWorld<WorldReal>();
  this->saveData(world.getPhysicsWorldView(), date);
}
template <typename Real, typename WorldReal>
void DataValidator<Real, WorldReal>::sendTable(Scene &scene)
{
  if (this->historyIdx == 0)
    return;

  std::cout << "Print table for all bodies (Y/N)? " << std::endl;
  char choice;
  std::cin >> choice;
  if (choice != 'Y' && choice != 'y')
    return;

  SimulationWorld<WorldReal> &world = scene.getSimulationWorld<WorldReal>();
  const EntityManager &manager = world.getEntityManager();

  for (const auto &[index, entity] : this->indexToEntity)
  {
    std::cout << std::scientific << std::setprecision(20);
    std::cout << std::endl;
    // position - m, velocity - m/s
    std::cout << "Body: " << manager.getEntityName(entity) << std::endl;
    std::cout << "|    Date    |     X      |     Y      |     Z      |     VX     |     VY     |     VZ     |" << std::endl;
    std::cout << "|------------|------------|------------|------------|------------|------------|------------|" << std::endl;
    for (size_t i = 0; i < this->historyIdx; ++i)
    {
      DataBodySample<Real> &b = this->bodyHistory[i][index];
      std::cout << "| " << b.date.toString()
                << " | " << b.relativePosition.x
                << " | " << b.relativePosition.y
                << " | " << b.relativePosition.z
                << " | " << b.relativeVelocity.x
                << " | " << b.relativeVelocity.y
                << " | " << b.relativeVelocity.z
                << " |" << std::endl;
    }
  }
}

template <typename Real, typename WorldReal>
void DataValidator<Real, WorldReal>::saveTable(Scene &scene, const std::filesystem::path &folderPath)
{
  SimulationWorld<WorldReal> &world = scene.getSimulationWorld<WorldReal>();
  const EntityManager &manager = world.getEntityManager();

  for (const auto &[index, entity] : this->indexToEntity)
  {
    std::string name = manager.getEntityName(entity);
    std::filesystem::path bodyPath = folderPath / (name + "_data_history.csv");
    std::ofstream bodyFile(bodyPath);
    if (!bodyFile.is_open())
      Logger::logFatal("Data Validator", "Couldnt open body history data file");

    bodyFile << std::scientific << std::setprecision(std::numeric_limits<Real>::max_digits10);
    bodyFile << "date;X;Y;Z;VX;VY;VZ" << std::endl;

    for (size_t i = 0; i < this->historyIdx; ++i)
    {
      // position - m, velocity - m/s
      DataBodySample<Real> &b = this->bodyHistory[i][index];
      bodyFile << b.date.toString() << ';'
               << b.relativePosition.x << ';'
               << b.relativePosition.y << ';'
               << b.relativePosition.z << ';'
               << b.relativeVelocity.x << ';'
               << b.relativeVelocity.y << ';'
               << b.relativeVelocity.z
               << std::endl;
    }

    bodyFile.close();

    std::cout << name << " data saved to: " << bodyPath << std::endl;
  }
}
