#version 400 core

in vec4 color;
in float texCoord;
out vec4 fColor;
uniform sampler1D tex;


void main()
{
    fColor = texture(tex,texCoord);
}
