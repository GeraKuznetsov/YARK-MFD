  
#version 330 core

layout (location = 0) in vec3 InVert;
layout (location = 1) in float InStrength;
uniform mat4 proj;
uniform mat4 view;

out float strength;

void main()
{
   gl_Position =  proj * view * vec4(InVert, 1); 
   strength = InStrength;
}