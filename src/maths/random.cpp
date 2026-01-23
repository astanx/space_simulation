#include "maths/random.h"

#include <random>

double generateRandom(double min, double max)
{
  thread_local std::mt19937 rng{std::random_device{}()};
  std::uniform_real_distribution<double> dist(min, max);
  return dist(rng);
}