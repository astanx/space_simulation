#pragma once

#include "scene/shadow/shadow.h"

class DirectionalShadow : public Shadow
{
protected:
  void init() override;

public:
  DirectionalShadow(const GLuint width, const GLuint height);
  ~DirectionalShadow() = default;

  void bind(Shader &shader, int textureUnit) const override;
};
