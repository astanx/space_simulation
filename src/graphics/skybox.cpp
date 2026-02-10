#include "graphics/skybox.h"

#include "debug/logger.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"

#include "graphics/primitives/cube.h"

#include <GL/glew.h>
#include "external/stb_image.h"

#include <iostream>

// Private functions
void Skybox::loadCubemap(std::vector<const char *> faces)
{
  glGenTextures(1, &this->id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);

  for (unsigned int i = 0; i < faces.size(); i++)
  {
    int channels;
    unsigned char *image = stbi_load(faces[i], &width, &height, &channels, 4);
    if (image)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                   0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
      stbi_image_free(image);

      GLenum err = glGetError();
      if (err != GL_NO_ERROR && i != 0)
        Logger::logError("Skybox", "Error loading face - " + std::to_string(i));
    }
    else
    {
      std::cout << "[Skybox] ERROR: Cubemap texture failed to load at path - " << faces[i] << std::endl;
      stbi_image_free(image);
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
};

// Constructor/Destructor
Skybox::Skybox(std::vector<const char *> &faces) : mesh(std::make_unique<Cube>(), VertexLayout::PositionOnly)
{
  if (faces.size() != 6)
    Logger::logFatal("Skybox", "Constructor requires 6 faces");

  this->loadCubemap(faces);
}

Skybox::~Skybox()
{
  glDeleteTextures(1, &this->id);
}

// Public functions
void Skybox::render(Shader &shader) const
{
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_FALSE);

  this->bind(0);
  shader.set1i(0, "skybox");
  this->mesh.render();
  this->unbind();

  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
}
void Skybox::bind(const GLint textureUnit) const
{
  if (glIsTexture(this->id))
  {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);
  }
  else
    Logger::logError("Skybox", "No texture to bind");
}

void Skybox::unbind() const
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}