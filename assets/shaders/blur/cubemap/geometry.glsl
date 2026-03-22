#version 410
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

vec3 faceDirections[6] = vec3[](
  vec3(1.0, 0.0, 0.0),  // +X
  vec3(-1.0, 0.0, 0.0), // -X
  vec3(0.0, 1.0, 0.0),  // +Y
  vec3(0.0, -1.0, 0.0), // -Y
  vec3(0.0, 0.0, 1.0),  // +Z
  vec3(0.0, 0.0, -1.0)  // -Z
);

out vec3 gs_position;

void main()
{
  for(int face = 0; face < 6; ++face)
  {
    gl_Layer = face;
    for(int i = 0; i < 3; ++i) 
    {
      gs_position = gl_in[i].gl_Position.xyz;
      EmitVertex();
    }    
    EndPrimitive();
  }
} 