#version 330 core

layout (location = 0) in vec2 InVert;
layout (location = 1) in vec2 InText;
out vec2 uv;
uniform mat4 orthro;
uniform mat4 view;

void main()
{
   gl_Position = orthro * view * vec4(InVert,0, 1); 
   uv = InText;  
}