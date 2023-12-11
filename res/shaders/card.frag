#version 330

uniform sampler2D tex;
varying vec2 UV;

void main()
{
    gl_FragColor.rgba = texture2D(tex, UV).rgba;
}