#include "model.h"

#include "render/mesh/mesh.h"
#include "render/material/material.h"
#include "render/texture/texture.h"
#include "render/shader/shader.h"

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

  for (auto& mesh : this->meshes)
  {
    mesh->move(this->position);
  }
}

Model::~Model()
{
  for (auto *&mesh : this->meshes)
  {
    delete mesh;
  }
}

// Public functions
void Model::update()
{
}
void Model::render(Shader *shader)
{
  shader->use();
  // Update uniforms
  this->updateUniforms(shader);

  // Render objects
  this->overrideTextureDiffuse->bind(0);
  this->overrideTextureSpecular->bind(1);

  for (auto &mesh : this->meshes)
  {
    mesh->render(shader);
  }

  // Unbind everything
  glBindVertexArray(0);
  this->overrideTextureDiffuse->unbind();
  this->overrideTextureSpecular->unbind();
  shader->unuse();
}