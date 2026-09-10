#pragma once

#include <filesystem>
#include <stddef.h>

struct ValidatorConfig
{
  bool pathSpecified = false;
  std::filesystem::path savePath; // --save folder
  size_t steps = 1;               // --steps steps
};