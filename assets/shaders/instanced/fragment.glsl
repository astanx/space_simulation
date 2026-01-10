#version 410

out vec4 fs_color;


in VS_OUT {
  vec3 vs_position;
  vec3 vs_color;
  vec2 vs_texcoord;
 vec3 vs_normal;
} fs_in;

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

struct DirLight {
  vec3 direction;
  float intensity;  
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};  
uniform DirLight dirLight;
uniform vec3 camPosition; 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * texture(material.diffuseTexture, fs_in.vs_texcoord).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuseTexture, fs_in.vs_texcoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specularTexture, fs_in.vs_texcoord).rgb;
    return (ambient + diffuse + specular) * light.intensity;
} 

void main()
{
  vec3 normal = normalize(fs_in.vs_normal);
  vec3 viewDir = normalize(camPosition - fs_in.vs_position);
  fs_color = vec4(CalcDirLight(dirLight, normal, viewDir), 1.0);
}