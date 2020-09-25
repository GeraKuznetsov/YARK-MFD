#version 330 core
#extension GL_EXT_texture_array : enable

uniform sampler2DArray tex;
uniform float tileWidth;
uniform float tileHeight;
in vec2 uv;
out vec4 FragColor;

void main()
{
	vec2 uv2 = uv * vec2(tileWidth, tileHeight);
	vec2 texXY = floor(uv2);
	vec2 inTex  = fract(uv2);
	float ID = mod(texXY.y, tileHeight) * tileWidth + mod(texXY.x,tileWidth);	
	FragColor = vec4(texture2DArray(tex, vec3(inTex, ID)).xyz, 1);
}
