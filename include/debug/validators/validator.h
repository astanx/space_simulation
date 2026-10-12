#pragma once

#include <string>

class Scene;
struct Date;

class Validator
{
public:
  Validator() = default;
  virtual ~Validator() = default;

  virtual void init(Scene &scene, Date date, size_t steps) = 0;
  virtual void update(Scene &scene, Date date) = 0;
  virtual void sendTable(Scene &scene) = 0;
  virtual void saveTable(Scene &scene, const std::filesystem::path &folderPath) = 0;
  virtual bool isFinished() = 0;
};