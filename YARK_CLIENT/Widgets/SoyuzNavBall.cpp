#include "SoyuzNavball.h"
#include "gtc\matrix_transform.hpp"

SoyuzNavBall::SoyuzNavBall() {

}
void SoyuzNavBall::LoadNavBallTextures() {

}

#define CROSS_WIDTH 1
#include <iostream>

std::string SoyuzNavBall::GetTitle() {
	return "Soyuz NavBall";
}

void SoyuzNavBall::Draw(XY pos, XY size) {
	VesselPacket VP = client.Vessel;

	if (lastSOI != VP.CurrentOrbit.SOINumber) {
		textureID = TL.getPlanetTexture(lastSOI = VP.CurrentOrbit.SOINumber);
	}

	float rad = min(size.x, size.y) / 2;

	glm::mat4 modelMat = glm::mat4(1);
	modelMat = glm::translate(modelMat, glm::vec3(pos.x + size.x / 2, pos.y + size.y / 2, 1));
	modelMat = glm::scale(modelMat, glm::vec3(rad / BORDER_SCALE, rad / BORDER_SCALE, rad / BORDER_SCALE));

	glm::mat4 rotMat = glm::mat4(1);
	rotMat = glm::rotate(rotMat, glm::radians(-VP.Lon + 90), glm::vec3(0, 0, 1));
	rotMat = glm::rotate(rotMat, glm::radians(VP.Lat - 90.f), glm::vec3(1, 0, 0));
	rotMat = rotMat * glm::rotate(glm::mat4(1), glm::radians(VP.Prograde.Heading), glm::vec3(0, 0, 1));

	NavBall::SD->DrawSphere(textureID, modelMat, rotMat);

	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(1, 1, 1);
	draw->SetDrawColor2D(0, 0, 1, 0.5); //DRAW CROSSHAIRS
	draw->DrawRect2D(pos.x, pos.y + size.y / 2 - CROSS_WIDTH, pos.x + size.x, pos.y + size.y / 2 + CROSS_WIDTH);
	draw->SetDrawColor2D(1, 0, 0, 0.5);
	draw->DrawRect2D(pos.x + size.x / 2 - CROSS_WIDTH, pos.y, pos.x + size.x / 2 + CROSS_WIDTH, pos.y + size.y);
}