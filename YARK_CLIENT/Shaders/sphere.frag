
#version 330 core

uniform sampler2D tex;
uniform mat4 rot;
uniform mat4 roll;
in vec2 pos;
out vec4 FragColor;

#define PI 3.1415926535897932384626433832795

float atan2(in float y, in float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI/2.0 - atan(x,y), atan(y,x), s);
}

void main(){
	float r2 = pos.x * pos.x + pos.y * pos.y;
	if(r2>=1){discard;}
	//FragColor = vec4(0,1,1,1);

	vec4 pos3D = vec4(pos, sqrt(1 - r2), 1);

	pos3D = roll * pos3D;
	pos3D = rot * pos3D;

	vec2 uv = vec2(atan2(pos3D.y,pos3D.x)/(PI),acos(pos3D.z/1)/(PI));  
	
	//float s = sin(roll);
	//float c = cos(roll);
	//uv-=vec2(.5f,0.5f);
	//uv = vec2(uv.x*c-uv.y*s,uv.x*s+uv.y*c);
	//uv+=vec2(.5f,0.5f);

	uv.x = uv.x / 2;
	uv.x = 1 - uv.x;
	uv.y = uv.y/2;
		
	FragColor = texture2D(tex,uv);
}

