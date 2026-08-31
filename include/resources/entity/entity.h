#pragma once

#include <stdio.h>

struct Entity
{
  size_t id;
  Entity() = default;
  explicit Entity(size_t id) : id(id) {}
};