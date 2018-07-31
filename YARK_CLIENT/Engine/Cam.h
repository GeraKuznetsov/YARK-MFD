#pragma once
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4x4.hpp"

class Cam {
public:
	float pitch, yaw;
	glm::vec3 pos;
	glm::vec4 viewPort;
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 PV;
	float fov;
	glm::vec3 Front;
	bool orthro;
	Cam(float pitch, float yaw, glm::vec3 pos);
	Cam();
	void Calculate();
	void SetViewPort(int x, int y, int width, int height);
	glm::vec4 ProjPoint(glm::vec3 p);
	glm::vec3 CreateRay(int mX, int mY);
};