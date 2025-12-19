#version 410

struct Material
{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  sampler2D diffuseTexture;
  sampler2D specularTexture;
  float shininess;
};

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

uniform Material material;

uniform bool isTexture;
uniform vec3 lightPos0;
uniform vec3 camPosition; 

void main()
{
  vec3 posToLightDirVec = normalize(vs_position - lightPos0);
  float diffuse = clamp(dot(posToLightDirVec, vs_normal), 0, 1);
  vec3 diffuseLight = material.diffuse * diffuse;

  vec3 lightToPosDirVec = normalize(lightPos0 - vs_position);
  vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
  vec3 posToViewDirVec = normalize(vs_position - camPosition);
  float specular = pow(max(dot(posToViewDirVec, reflectDirVec), 0), material.shininess);
  vec3 specularLight = material.specular * specular;

  if (isTexture) fs_color = texture(material.diffuseTexture, vs_texcoord) * vec4(vs_color, 1.f);
  else fs_color = vec4(vs_color, 1.f);
  
  fs_color *= (vec4(material.ambient, 1.f) + vec4(diffuseLight, 1.f) + vec4(specularLight, 1.f));
}