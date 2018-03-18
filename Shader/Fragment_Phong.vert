#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vNormal;
uniform mat4 mvp_matrix;
uniform mat4 normal_matrix;
uniform mat4 modelview_matrix;

out vec3 norm;
out vec3 pos;

mat4 billboard=modelview_matrix;

void main()
{

    norm=normalize(vec4(normal_matrix*vNormal).xyz);

    pos=-normalize(vec4(modelview_matrix*vPosition).xyz);
    gl_Position = mvp_matrix*vPosition;
}
