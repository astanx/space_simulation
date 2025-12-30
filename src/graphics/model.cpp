#include "graphics/model.h"
#include "graphics/mesh.h"
#include "graphics/material.h"
#include "graphics/texture.h"
#include "graphics/shader.h"

#include "utils/OBJloader.h"

// Private functions
void Model::updateUniforms(Shader *shader)
{
  this->material->sendToShader(*shader);
}

// Constructor/Descructor
Model::Model(glm::vec3 position, Material *material,
             std::vector<Mesh *> &meshes,
             Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;

  for (auto *mesh : meshes)
  {
    this->meshes.push_back(new Mesh(*mesh));
  }

  for (auto &mesh : this->meshes)
  {
    mesh->move(this->position);
  }
}
Model::Model(glm::vec3 position, Material *material,
        const char* OBJfile,
        Texture *overrideTextureDiffuse, Texture *overrideTextureSpecular)
{
  this->position = position;
  this->material = material;
  this->overrideTextureDiffuse = overrideTextureDiffuse;
  this->overrideTextureSpecular = overrideTextureSpecular;

  std::vector<Vertex> vercites = loadOBJmodel(OBJfile);

  this->meshes.push_back(new Mesh(vercites.data(), vercites.size(), NULL, 0, this->position, this->position));
}

Model::~Model()
{
  for (auto *&mesh : this->meshes)
  {
    delete mesh;
  }
}

// Public functions
void Model::render(Shader *shader)
{
  shader->use();
  
  // Update uniforms
  this->updateUniforms(shader);

  // Render objects
  if (overrideTextureDiffuse != nullptr)
    this->overrideTextureDiffuse->bind(0);
  if (overrideTextureSpecular != nullptr)
    this->overrideTextureSpecular->bind(1);

  for (auto &mesh : this->meshes)
  {
    mesh->render(shader);
  }

  // Unbind everything
  glBindVertexArray(0);
  if (overrideTextureDiffuse != nullptr)
    this->overrideTextureDiffuse->unbind();
  if (overrideTextureSpecular != nullptr)
    this->overrideTextureSpecular->unbind();
  shader->unuse();
}
void Model::scaleBy(const glm::vec3 scale)
{
  for (auto& mesh : meshes)
  {
    mesh->scaleBy(scale);
  }
}