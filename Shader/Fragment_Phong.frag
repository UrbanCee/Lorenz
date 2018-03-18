#version 400 core

out vec4 fColor;

in vec3 norm;
in vec3 pos;

uniform vec3 mat[4];
uniform float mat_shininess;

void main()
{
    float ndp=max(0.0,dot(normalize(norm),normalize(pos)));

    vec3 diffuse=mat[2]*ndp;
    vec3 specular=mat[3]*pow(ndp,mat_shininess);
    vec3 ambient=mat[1]*0.3;

    vec3 finalcol=ambient+diffuse+specular;
    fColor=vec4(finalcol,1.0);
}
