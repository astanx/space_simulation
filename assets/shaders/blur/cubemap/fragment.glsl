#version 410
out vec4 fs_color;
  
in vec3 gs_position;

uniform samplerCube cube;

const int MAX_KERNEL_SIZE = 40;
uniform int kernel_size;
  
uniform bool horizontal;
uniform float weight[MAX_KERNEL_SIZE];

void main()
{         
  vec3 dir = normalize(gs_position);

  float resolution = float(textureSize(cube, 0));
  float offset = 1.0 / resolution;

  vec3 result = texture(cube, dir).rgb * weight[0];

  for (int i = 1; i < kernel_size; ++i)
  {
    float scale = offset * float(i);

    if (horizontal)
    {
      result += texture(cube, dir + vec3(scale, 0.0, 0.0)).rgb * weight[i];
      result += texture(cube, dir - vec3(scale, 0.0, 0.0)).rgb * weight[i];
    }
    else
    {
      result += texture(cube, dir + vec3(0.0, scale, 0.0)).rgb * weight[i];
      result += texture(cube, dir - vec3(0.0, scale, 0.0)).rgb * weight[i];
    }
  }

	fs_color = vec4(result, 1.0);
}