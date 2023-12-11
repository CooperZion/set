#version 330

attribute vec3 coord;
attribute vec2 texCoord;

varying vec2 UV;

void main()
{
    gl_Position = ftransform();
    UV = texCoord;
}