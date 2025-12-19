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

struct Light
{
  vec3 position;
  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

uniform Material material;
uniform Light light;

uniform bool isTexture;
uniform vec3 camPosition; 

vec3 calculateAmbient()
{
  return material.ambient * light.ambient;
}

vec3 calculateDiffuse()
{
  vec3 posToLightDirVec = normalize(vs_position - light.position);
  float diffuse = clamp(dot(posToLightDirVec, vs_normal), 0, 1);
  return material.diffuse * diffuse * light.diffuse;
}

vec3 calculateSpecular()
{
  vec3 lightToPosDirVec = normalize(light.position - vs_position);
  vec3 reflectDirVec = normalize(reflect(lightToPosDirVec, normalize(vs_normal)));
  vec3 posToViewDirVec = normalize(vs_position - camPosition);
  float specular = pow(max(dot(posToViewDirVec, reflectDirVec), 0), material.shininess);
  return material.specular * specular * light.specular;
}

void main()
{
  vec3 ambientLight = calculateAmbient();

  vec3 diffuseLight = calculateDiffuse();

  vec3 specularLight = calculateSpecular();

  vec4 baseColor = isTexture 
  ? texture(material.diffuseTexture, vs_texcoord) * vec4(vs_color, 1.f) 
  : vec4(vs_color, 1.f);
  
  fs_color = baseColor * (vec4(ambientLight, 1.f) + vec4(diffuseLight, 1.f) + vec4(specularLight, 1.f));
}