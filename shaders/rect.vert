#version 330 core
layout (location = 0) in vec2 aPointPos;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vec4 pos = projectionMatrix * modelMatrix * vec4(aPointPos, 0.0f, 1.0f);
    gl_Position = pos;
}
