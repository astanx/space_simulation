#include "graphics/skybox.h"

#include "debug/logger.h"

#include "graphics/mesh.h"
#include "graphics/shader.h"

#include "graphics/bindings/texture.h"

#include "graphics/state/scopedDepthMask.h"
#include "graphics/state/scopedDepthFunc.h"
#include "graphics/state/scopedTexture.h"

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
      GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                           0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
      stbi_image_free(image);
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

  GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

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
  ScopedDepthFunc depthLequal(GL_LEQUAL);
  ScopedDepthMask depthMask(GL_FALSE);
  ScopedTexture skyboxText(this->id, static_cast<GLenum>(GL_TEXTURE_CUBE_MAP), TextureBindingPoints::Skybox);

  shader.set1i(TextureBindingPoints::Skybox, "skybox");

  this->mesh.render();
}