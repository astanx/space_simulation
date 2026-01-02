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
uniform Material material;


struct PointLight
{
  vec3 position;
  float intensity;
  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};
uniform PointLight pointLight;


struct DirLight {
  vec3 direction;
  float intensity;  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};  
uniform DirLight dirLight;

in vec3 vs_position;
in vec3 vs_color;
in vec2 vs_texcoord;
in vec3 vs_normal;

out vec4 fs_color;

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

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * getAlbedo();
    vec3 diffuse  = light.diffuse  * diff * getAlbedo();
    vec3 specular = light.specular * spec * getSpecularMap();
    return (ambient + diffuse + specular) * light.intensity;
} 

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (
    light.constant +
    light.linear * distance +
    light.quadratic * distance * distance
  );

  vec3 ambient  = light.ambient * getAlbedo();
  vec3 diffuse  = light.diffuse * diff * getAlbedo();
  vec3 specular = light.specular * spec * getSpecularMap();

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular) * light.intensity;
} 

void main()
{
  // if (texture(material.diffuseTexture, vs_texcoord).a < 0.1) discard;
  vec3 normal = normalize(vs_normal);
  vec3 viewDir = normalize(camPosition - vs_position);

  vec3 result = CalcDirLight(dirLight, normal, viewDir);
  result += CalcPointLight(pointLight, normal, vs_position, viewDir);
  fs_color = vec4(result, 1.0);
}