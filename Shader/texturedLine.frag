#version 400 core

in float texCoord;
uniform float texOffset;
out vec4 fColor;
uniform sampler1D tex;
uniform sampler1D highlight;


void main()
{
    fColor = 0.9*texture(tex,texCoord+texOffset)+texture(highlight,texCoord*0.1+texOffset);
}
