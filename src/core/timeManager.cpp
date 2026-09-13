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
void TimeManager::update(bool isPaused)
{
  double currentFrame = this->getTime();
  this->deltaTime = currentFrame - this->lastFrame;
  this->lastFrame = currentFrame;

  if (this->isFirstFrame)
  {
    this->deltaTime = 0.0;
    this->isFirstFrame = false;
  }

  if (!isPaused)
    this->elapsedDays += this->deltaTime * this->cfg.timestep / 86400.0;

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
