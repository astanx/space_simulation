#version 410

layout (location = 0) in vec3 vs_position;

out vec3 vs_texcoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main()
{
    vs_texcoord = vs_position;
    mat4 ViewMatrixNoTranslation = mat4(mat3(ViewMatrix));
    vec4 pos = ProjectionMatrix * ViewMatrixNoTranslation * vec4(vs_position, 1.0);
    gl_Position = pos.xyww;
}  