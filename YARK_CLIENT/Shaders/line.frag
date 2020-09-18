#version 330 core

in float strength;
uniform vec4 color;
out vec4 FragColor;

void main()
{
    FragColor = strength * color;
}