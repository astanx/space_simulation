#pragma once

#include <filesystem>
#include <stddef.h>

enum class ValidatorType
{
  Data,
  Energy
};

struct ValidatorConfig
{
  ValidatorType type = ValidatorType::Energy;
  bool pathSpecified = false;
  size_t steps = 1;                        // --steps steps
  Precision precision = Precision::DOUBLE; // --validator-precision <float|double>
  std::filesystem::path savePath;          // --save folder
};