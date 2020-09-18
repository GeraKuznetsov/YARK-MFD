#version 330 core

layout (location = 0) in vec3 InVert;
layout (location = 1) in vec2 InText;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec2 uv;

void main()
{
    gl_Position =  proj * view * model * vec4(InVert, 1); 
    uv = InText;  
}
