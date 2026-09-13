#pragma once

#include "graphics/texture.h"
#include "graphics/mesh.h"
#include "graphics/materials/textMaterial.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <map>

struct Character
{
  std::unique_ptr<Texture> texture;
  glm::ivec2 size;
  glm::ivec2 bearing;
  unsigned int advance;
};

class TextRenderer
{
private:
  std::map<char, Character> characters;
  std::optional<TextMaterial> mat;

  std::unique_ptr<Mesh> text;

public:
  TextRenderer() = default;
  ~TextRenderer() = default;

  void init();

  void render(Shader &shader, std::string text, float x, float y, float scale = 1.0, glm::vec3 color = glm::vec3(1.0));
};