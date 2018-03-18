
#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vCol;
uniform mat4 mvp_matrix;
out vec4 color;

void
main()
{
    color=vCol;
    gl_Position = mvp_matrix*vPosition;
}
