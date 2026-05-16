#pragma once

#include "graphics/texture.h"

class AreaLight
{
private:
  std::unique_ptr<Texture> LTC1;
  std::unique_ptr<Texture> LTC2;

  void initLTC(float resolution);

public:

};