#version 410
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
  vec3 vs_position;
  vec3 vs_color;
  vec2 vs_texcoord;
  vec3 vs_normal;
} gs_in[];

const float MAGNITUDE = 0.4;
  

void GenerateLine(int index)
{
    gl_Position =  gl_in[index].gl_Position;
    EmitVertex();
    gl_Position =  (gl_in[index].gl_Position + 
                                vec4(gs_in[index].vs_normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}