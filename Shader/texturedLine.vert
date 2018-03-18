
#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vCol;
layout( location = 2 ) in float fTexCoord;
uniform mat4 mvp_matrix;
uniform float texOffset;
out vec4 color;
out float texCoord;

void
main()
{
    color=vCol;
    color.r=fTexCoord;
    color.g=fTexCoord;
    color.b=fTexCoord;
    texCoord=fTexCoord+texOffset;
    gl_Position = mvp_matrix*vPosition;
}
