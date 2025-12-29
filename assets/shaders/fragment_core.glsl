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

  float constant;
  float linear;
  float quadratic;
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


vec3 getAlbedo()
{
  return isTexture
  ? texture(material.diffuseTexture, vs_texcoord).rgb
  : vs_color;
}

vec3 getSpecularMap()
{
  return isTexture
  ? texture(material.specularTexture, vs_texcoord).rgb
  : vec3(1.0);
}

vec3 calculateAmbient()
{
  return light.ambient * getAlbedo();
}

vec3 calculateDiffuse()
{
  vec3 lightDir = normalize(light.position - vs_position);
  float diffuse = max(dot(normalize(vs_normal), lightDir), 0.0);
  return light.diffuse * diffuse * getAlbedo();
}

vec3 calculateSpecular()
{
  vec3 lightToPosDirVec = normalize(light.position - vs_position);
  vec3 reflectDirVec = reflect(-lightToPosDirVec, normalize(vs_normal));
  vec3 viewDir = normalize(camPosition - vs_position);
  float specular = pow(max(dot(viewDir, reflectDirVec), 0.0), material.shininess);
  return light.specular * specular * getSpecularMap();
}

void main()
{
  vec3 ambientLight = calculateAmbient();

  vec3 diffuseLight = calculateDiffuse();

  vec3 specularLight = calculateSpecular();

  float distance = length(light.position - vs_position);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

  ambientLight *= attenuation;
  diffuseLight *= attenuation;
  specularLight *= attenuation; 
  
  fs_color = vec4(ambientLight, 1.f) + vec4(diffuseLight, 1.f) + vec4(specularLight, 1.f);
}