#version 330 core

uniform sampler2D tex;
uniform mat4 rot;
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

	vec4 col;
	
	if(r2<1){
		vec4 pos3D = rot *vec4(pos, sqrt(1 - r2), 1);
		
		vec2 uv = vec2(atan2(pos3D.y,pos3D.x)/(PI),acos(pos3D.z/1)/(PI));  
			
		uv.x = uv.x / 2;
		uv.x = 1 - uv.x;
		uv.y = uv.y/2;
			
		col = texture2D(tex,uv);

	}else if(r2<sqrt(1.1)){
		col = vec4(0,0,0,1);
	}else if(r2<sqrt(1.2)){
		col = vec4(1,1,1,1);
	}else{
		discard;
	}
	FragColor = col;
}

