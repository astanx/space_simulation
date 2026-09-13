#pragma once

#include <cstdint>

enum class ModelFlags : uint32_t
{
  None = 0,
  CastsShadow = 1 << 0,
  ReflectsLight = 1 << 1,
  Special = 1 << 2,
};

inline ModelFlags operator|(ModelFlags a, ModelFlags b)
{
  return static_cast<ModelFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline ModelFlags operator&(ModelFlags a, ModelFlags b)
{
  return static_cast<ModelFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}