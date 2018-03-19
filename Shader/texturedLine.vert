
#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in float fTexCoord;
uniform mat4 mvp_matrix;
out float texCoord;

void
main()
{
    texCoord=fTexCoord;
    gl_Position = mvp_matrix*vPosition;
}
