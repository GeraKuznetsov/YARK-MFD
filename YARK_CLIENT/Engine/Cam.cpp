#include "Cam.h"
#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "geometric.hpp"
#include "gtc/matrix_transform.hpp"

Cam::Cam(float pitch, float yaw, glm::vec3 pos) {
	this->pitch = pitch;
	this->yaw = yaw;
	this->pos = pos;
}
void Cam::SetViewPort(int x, int y, int width, int height) {
	viewPort[0] = x;
	viewPort[1] = y;
	viewPort[2] = width;
	viewPort[3] = height;
}

Cam::Cam() {}

void Cam::Calculate() {
	if (orthro) {
		projection = glm::ortho(0.f, viewPort[2], viewPort[3], 0.f, -1000.f, 1000.f);
	}
	else {
		projection = glm::perspective(fov / viewPort[2] * viewPort[3], viewPort[2] / viewPort[3], 0.1f, 1000.0f);
	}

	Front.x = cos(yaw) * cos(-pitch);
	Front.y = sin(-pitch);
	Front.z = sin(yaw) * cos(-pitch);
	Front = glm::normalize(Front);
	// Also re-calculate the Right and Up vector
	glm::vec3 Right = glm::normalize(glm::cross(Front, glm::vec3{ 0.f,1.f,0.f }));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	glm::vec3 Up = glm::normalize(glm::cross(Right, Front));

	view = glm::lookAt(pos, pos + Front, Up);

	PV = projection * view;
}

glm::vec4 Cam::ProjPoint(glm::vec3 p) {
	glm::vec4 foo = PV * glm::vec4(p, 1.f);
	foo /= foo.w;
	foo.x = (foo.x* 0.5 + 0.5) * viewPort[2] + viewPort[0];
	foo.y = (foo.y* -0.5 + 0.5) * viewPort[3] + viewPort[1];
	return foo;
}

glm::vec3 Cam::CreateRay(int mX, int mY) {
	float mouseX = (float(mX)) / (viewPort[2] * 0.5f) - 1.0f;
	float mouseY = (float(mY)) / (viewPort[3] * 0.5f) - 1.0f;

	glm::mat4 invVP = glm::inverse(PV);
	glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
	glm::vec4 worldPos = invVP * screenPos;

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

	return  glm::normalize(glm::vec3(worldPos));
}