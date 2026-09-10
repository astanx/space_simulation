#pragma once

#include <stdio.h>

struct Entity
{
  size_t id;
  Entity() = default;
  Entity(const Entity& other) = default;
  explicit Entity(size_t id) : id(id) {}
};