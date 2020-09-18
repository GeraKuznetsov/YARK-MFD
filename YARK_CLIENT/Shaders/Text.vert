#version 330 core

layout (location = 0) in vec2 InVert;
layout (location = 1) in vec2 InText;
layout (location = 2) in uint InID;

out vec2 uv;
flat out uint ID;

uniform mat4 orthro;
uniform mat4 view;

void main()
{
   gl_Position = orthro * view * vec4(InVert, 0, 1); 
   uv = InText;  
   ID = InID;
}