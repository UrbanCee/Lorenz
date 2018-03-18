#version 400 core

layout( location = 0 ) in vec4 vPosition;
uniform mat4 mvp_matrix;
out vec4 col;

void
main()
{
    col=vec4(1.0,0.5,0.0,0.0);
    gl_Position = mvp_matrix*vPosition;
}
