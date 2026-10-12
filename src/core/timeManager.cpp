#include "core/time/timeManager.h"

#include "debug/logger.h"

#include "core/time/timeConfig.h"

#include "maths/dateToJD.h"

// Private functions

// Constructor / Desctructor
TimeManager::TimeManager(const TimeConfig &cfg) : cfg(cfg)
{
  this->deltaTime = 0.0;
  this->clock = std::chrono::steady_clock::now();
  this->lastFrame = this->getTime();

  this->startTime = dateToJD(this->cfg.startDate);
};

TimeManager::~TimeManager() = default;

// Public functions
void TimeManager::update(bool isPaused, bool useDt)
{
  double currentFrame = this->getTime();
  this->deltaTime = currentFrame - this->lastFrame;
  this->lastFrame = currentFrame;

  if (this->isFirstFrame)
  {
    this->reset();
    this->isFirstFrame = false;
  }

  if (!isPaused)
  {
    if (useDt)
      this->elapsedDays += this->deltaTime * this->cfg.timestep / 86400.0;
    else
      this->elapsedDays += this->deltaTime != 0 ? this->cfg.timestep / 86400.0 : 0;
  }

  // Update FPS counter
  this->frames++;
  double elapsed = currentFrame - lastFpsUpdateTime;
  if (elapsed >= 1.0)
  {
    this->fps = frames / elapsed;

    Logger::logInfo("FPS", std::to_string(fps));

    this->frames = 0;
    this->lastFpsUpdateTime = currentFrame;
  }
};
void TimeManager::reset()
{
  this->clock = std::chrono::steady_clock::now();

  double now = this->getTime();

  this->lastFrame = now;
  this->lastFpsUpdateTime = now;
  this->deltaTime = 0.0;

  this->elapsedDays = 0.0;
  this->frames = 0;
  this->isFirstFrame = true;
};
double TimeManager::getTime()
{
  return std::chrono::duration<double>(std::chrono::steady_clock::now() - this->clock).count();
};
Date TimeManager::getDate()
{
  return JDToDate(this->startTime + this->elapsedDays);
}
double TimeManager::getDeltaTime()
{
  return this->deltaTime * this->cfg.timestep;
};
