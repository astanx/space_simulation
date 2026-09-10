#pragma once

#include <string>

class Scene;

class Validator
{
public:
  Validator() = default;
  virtual ~Validator() = default;

  virtual void init(Scene &scene, double elapsedTime, size_t steps) = 0;
  virtual void update(Scene &scene, double elapsedTime) = 0;
  virtual void sendTable() = 0;
  virtual void saveTable(Scene &scene, const std::filesystem::path &folderPath) = 0;
  virtual bool isFinished() = 0;
};