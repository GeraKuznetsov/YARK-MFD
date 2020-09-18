#version 330 core

#extension GL_EXT_texture_array : enable

uniform sampler2DArray tex;
uniform vec4 color;

in vec2 uv;
flat in uint ID;

out vec4 FragColor;

void main()
{
	vec4 col = color;
	col.a = texture2DArray(tex, vec3(uv, ID)).r *  col.a;
	FragColor = col;
}
