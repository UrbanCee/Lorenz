#version 400 core

in vec4 color;
in float texCoord;
uniform float texOffset;
out vec4 fColor;
uniform sampler1D tex;
uniform sampler2D highlight;


void main()
{
    fColor = 0.9*texture(tex,texCoord+texOffset)+texture2D(highlight,vec2(texCoord*0.1+texOffset,0.0));
}
