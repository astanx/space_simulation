#pragma once

#include "resources/data/date.h"

struct TimeConfig
{
  double timestep = 86400;           // --timestep time
  Date startDate = Date{1, 1, 2000}; // --date day/month/year hour:minute:second
};