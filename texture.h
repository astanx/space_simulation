#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>

#include <OpenGL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>

class Texture
{
private:
  GLuint id;
  int width;
  int height;
  unsigned int type;
  GLint textureUnit;

public:
  Texture(const char* fileName, GLenum type, GLint textureUnit)
  { 
    this->type = type;
    this->textureUnit = textureUnit;
    unsigned char* image = SOIL_load_image(fileName, &this->width, &this->height, 0, SOIL_LOAD_RGBA);
    
    glGenTextures(1, &this->id);
    glBindTexture(type, this->id);

    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if(image)
    {
      glTexImage2D(type, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
      glGenerateMipmap(type);
    }
    else
    {
      std::cout << "Failed to load texture: " << fileName << std::endl;
    }

    glActiveTexture(0);
    glBindTexture(type, 0);
    SOIL_free_image_data(image);
  }

  ~Texture()
  {
    glDeleteTextures(1, &this->id); 
  }

  inline GLuint getId() const
  {
    return this->id;
  }

  void bind()
  {
    glActiveTexture(GL_TEXTURE0 + this->textureUnit);
    glBindTexture(this->type, this->id);
  }

  void unbind()
  {
    glActiveTexture(0);
    glBindTexture(this->type, 0);
  }

  inline GLint getTextureUnit() const 
  {
    return this->textureUnit;
  };

  void loadFromFile(const char* fileName)
  {
    if (this->id) glDeleteTextures(1, &this->id);
    unsigned char* image = SOIL_load_image(fileName, &this->width, &this->height, 0, SOIL_LOAD_RGBA);
    
    glGenTextures(1, &this->id);
    glBindTexture(this->type, this->id);

    glTexParameteri(this->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(this->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(this->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(this->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if(image)
    {
      glTexImage2D(this->type, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
      glGenerateMipmap(this->type);
    }
    else
    {
      std::cout << "Failed to load texture from file: " << fileName << std::endl;
    }

    glActiveTexture(0);
    glBindTexture(this->type, 0);
    SOIL_free_image_data(image);
  }
};