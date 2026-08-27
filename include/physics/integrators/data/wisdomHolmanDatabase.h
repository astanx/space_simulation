#pragma once

#include "physics/integrators/data/integratorDatabase.h"

template <typename Real>
struct WisdomHolmanDatabase : public IntegratorDatabase
{
private:
public:
  using IntegratorDatabase::IntegratorDatabase();
  ~WisdomHolmanDatabase() = default;
};