#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vColor;
uniform mat4 mvp_matrix;
out vec4 col;

void
main()
{
    col=vColor;
    gl_Position = mvp_matrix*vPosition;
}
