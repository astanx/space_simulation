#pragma once

#include <chrono>

struct Date;
struct TimeConfig;

class TimeManager
{
private:
  std::chrono::steady_clock::time_point clock;
  double deltaTime;
  double lastFrame;
  unsigned frames = 0;
  double lastFpsUpdateTime = 0.0;
  double elapsedDays = 0.0;
  double startTime = 0.0;
  bool isFirstFrame = true;

  const TimeConfig &cfg;

  float fps;

public:
  TimeManager(const TimeConfig &cfg);
  ~TimeManager();

  void update(bool isPaused);

  double getTime();
  Date getDate();
  double getDeltaTime();
  int getFPS() { return this->fps; };
  double getStartTime() { return this->startTime; };
  double getElapsedDays() { return this->elapsedDays; };
  double getElapsedTime() { return this->elapsedDays * 86400; };
  double getFrameDeltaTime() { return this->deltaTime; };
};