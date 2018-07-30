#version 330 core

uniform sampler2D tex;
uniform bool useTex;
uniform vec4 color;
in vec2 uv;
out vec4 FragColor;

void main()
{
	if(useTex){
		FragColor = texture2D(tex, uv) * color;
	}else{
		FragColor = color;
	}
}
