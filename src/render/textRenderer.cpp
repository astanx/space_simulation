#include "render/textRenderer.h"
#include "graphics/mesh.h"
#include "graphics/vertex.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

// Public functions
void TextRenderer::init()
{
  FT_Library ft;
  if (FT_Init_FreeType(&ft))
  {
    throw std::runtime_error("ERROR::TextRenderer::Could not init FreeType Library");
  }

  FT_Face face;
  if (FT_New_Face(ft, "assets/fonts/arial.ttf", 0, &face))
  {
    FT_Done_FreeType(ft);
    throw std::runtime_error("ERROR::TextRenderer::Failed to load font");
  }
  else
  {
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    for (unsigned char c = 0; c < 128; c++)
    {
      // load character glyph
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      {
        std::cout << "ERROR::TextRenderer::Failed to load Glyph" << std::endl;
        continue;
      }

      Character character = {
          std::make_unique<Texture>(face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_TEXTURE_2D, face->glyph->bitmap.buffer),
          glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
          static_cast<unsigned int>(face->glyph->advance.x)};

      this->characters[c] = std::move(character);
    }
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
};

void TextRenderer::render(Shader &shader, std::string text, float screenWidth, float screenHeight, float x, float y, float scale, glm::vec3 color)
{
  shader.use();
  // activate corresponding render state
  glm::mat4 projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
  shader.setVec3f(color, "textColor");

  shader.setMat4fv(projection, "projection");

  // iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++)
  {
    auto it = characters.find(*c);
    if (it == characters.end())
      continue;
    Character &ch = it->second;

    float xpos = x + ch.bearing.x * scale;
    float ypos = y - (ch.size.y - ch.bearing.y) * scale;

    float w = ch.size.x * scale;
    float h = ch.size.y * scale;
    // update VBO for each character
    std::vector<Vertex> vertices{
        {{xpos, ypos + h, 0.0f}, {}, {0.0f, 0.0f}},
        {{xpos, ypos, 0.0f}, {}, {0.0f, 1.0f}},
        {{xpos + w, ypos, 0.0f}, {}, {1.0f, 1.0f}},

        {{xpos, ypos + h, 0.0f}, {}, {0.0f, 0.0f}},
        {{xpos + w, ypos, 0.0f}, {}, {1.0f, 1.0f}},
        {{xpos + w, ypos + h, 0.0f}, {}, {1.0f, 0.0f}}};

    // render glyph texture over quad
    ch.texture->bind(0);

    shader.set1i(0, "text");

    Mesh mesh = Mesh(vertices.data(), vertices.size(), nullptr, 0, VertexLayout::PositionTexcoord);

    mesh.render();
    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)

    ch.texture->unbind();
  }
  shader.unuse();
}