#version 330 core

layout (location = 0) in vec2 InVert;

uniform mat4 proj;
uniform mat4 model;
out vec2 pos;

void main()
{
   gl_Position =  proj * model * vec4(InVert, 0, 1); 
   pos = InVert;
}
