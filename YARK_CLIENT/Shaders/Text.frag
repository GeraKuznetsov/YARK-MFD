#version 330 core

uniform sampler2D tex;
uniform vec4 color;
in vec2 uv;
out vec4 FragColor;

void main()
{
	vec4 col = color;
	col.a = texture2D(tex, uv).r *  col.a;
	FragColor = col;
}
