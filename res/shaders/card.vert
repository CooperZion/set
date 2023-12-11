#version 330

attribute vec3 coord;
attribute vec2 texCoord;

uniform mat4 model;
uniform mat4 projection;
layout (location = 0) in vec2 aPos;

varying vec2 UV;

void main()
{
    gl_Position =  projection * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
    UV = texCoord;
}