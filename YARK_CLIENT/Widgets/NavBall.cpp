#include "NavBall.h"
#include <iostream>

#define BORDER_SCALE 1.05f

NavBall::NavBall(WidgetStuff ws) : Widget(ws) {
		shader = LoadRawShader("Shaders/sphere.vert", "Shaders/sphere.frag");
	glUseProgram(shader);
	proj = glGetUniformLocation(shader, "proj");
	modelUnif = glGetUniformLocation(shader, "model");
	rotUnif = glGetUniformLocation(shader, "rot");

	GLfloat vertices[6][2] = {
		{ -1.f*BORDER_SCALE,-1.f*BORDER_SCALE },
	{ -1.f*BORDER_SCALE,1.f*BORDER_SCALE },
	{ 1.f*BORDER_SCALE,-1.f*BORDER_SCALE },
	{ 1.f*BORDER_SCALE,-1.f*BORDER_SCALE },
	{ -1.f*BORDER_SCALE,1.f*BORDER_SCALE },
	{ 1.f*BORDER_SCALE,1.f*BORDER_SCALE }
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	navballTex = loadTexture("Tex/navball/navball.png");
	chevron = loadTexture("Tex/navball/chevron.png");
	PGTex = loadTexture("Tex/navball/PG.png");
	RGTex = loadTexture("Tex/navball/RG.png");
	RITex = loadTexture("Tex/navball/RI.png");
	ROTex = loadTexture("Tex/navball/RO.png");
	NTex = loadTexture("Tex/navball/N.png");
	ANTex = loadTexture("Tex/navball/AN.png");
	TPGTex = loadTexture("Tex/navball/TPG.png");
	TRGTex = loadTexture("Tex/navball/TRG.png");
	MTex = loadTexture("Tex/navball/M.png");
}

#define GLM_ENABLE_EXPERIMENTAL
#include "gtc\matrix_transform.hpp"
#include "gtx/rotate_vector.hpp"

void renderNavHeading(NavHeading NH, VesselPacket *DI, Draw* draw, glm::mat4 *modelMat, GLuint Tex);
void NavBall::Tick(Draw* draw) {
	WindowUpdate(draw);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	VesselPacket VP = (*client) ? (*client)->vesselPacket : VesselPacket();

	float rad = (size.x - 50) / 2;

	glm::mat4 modelMat = glm::mat4(1);
	modelMat = glm::translate(modelMat, glm::vec3(pos.x + size.x / 2, pos.y + size.y / 2, 1));
	modelMat = glm::scale(modelMat, glm::vec3(rad / BORDER_SCALE, rad / BORDER_SCALE, rad / BORDER_SCALE));

	glm::mat4 rotMat = glm::mat4(1);
	rotMat = glm::rotate(rotMat, glm::radians(-VP.Heading + 90), glm::vec3(0, 0, 1));
	rotMat = glm::rotate(rotMat, glm::radians(VP.Pitch - 90), glm::vec3(1, 0, 0));
	rotMat = rotMat * glm::rotate(glm::mat4(1), glm::radians(-VP.Roll), glm::vec3(0, 0, 1));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, navballTex);
	glUseProgram(shader);
	SetShaderMat4(proj, draw->GetOrthroMat());

	SetShaderMat4(modelUnif, modelMat);

	SetShaderMat4(rotUnif, rotMat);

	glBindVertexArray(vao); //DRAW NAVBALL
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(1, 1, 1);

	draw->BindTex2D(chevron);  //DRAW CHEVRON
	draw->DrawRectUV2D(pos.x + size.x / 2 - 90, pos.y + size.y / 2 - 35, pos.x + size.x / 2 + 90, pos.y + size.y / 2 + 35, 0.f / 256.f, 0.f / 256.f, 180.f / 256.f, 70.f / 256.f);

	NavHeading NH = VP.Prograde;

	renderNavHeading(VP.Prograde, &VP, draw, &modelMat, PGTex);
	renderNavHeading(NavHeading(-VP.Prograde.Pitch, VP.Prograde.Heading + 180), &VP, draw, &modelMat, RGTex);

	if (VP.SpeedMode == 1) {
		renderNavHeading(NavHeading(90.f, 0), &VP, draw, &modelMat, ROTex);
		renderNavHeading(NavHeading(-90.f, 0), &VP, draw, &modelMat, RITex);

		renderNavHeading(NavHeading(0.f, VP.NormalHeading), &VP, draw, &modelMat, NTex);
		renderNavHeading(NavHeading(0.f, VP.NormalHeading + 180), &VP, draw, &modelMat, ANTex);
	}

	if (VP.MNDeltaV) {
		renderNavHeading(VP.Maneuver, &VP, draw, &modelMat, MTex);
	}
	if (VP.HasTarget) {
		renderNavHeading(VP.Target, &VP, draw, &modelMat, TPGTex);
		renderNavHeading(NavHeading(-VP.Target.Pitch, VP.Target.Heading + 180), &VP, draw, &modelMat, TRGTex);
	}
}

void renderNavHeading(NavHeading NH, VesselPacket *VP, Draw* draw, glm::mat4 *modelMat, GLuint Tex) {
	float tempHeading = glm::radians(NH.Heading);
	float tempPitch = glm::radians(NH.Pitch);
	glm::vec4 NavVector = glm::vec4(glm::cos(tempPitch) * glm::sin(tempHeading), -glm::sin(tempPitch), -glm::cos(tempPitch) * glm::cos(tempHeading), 1);
	NavVector = glm::rotateY(NavVector, glm::radians(VP->Heading));
	NavVector = glm::rotateX(NavVector, glm::radians(VP->Pitch));
	NavVector = glm::rotateZ(NavVector, glm::radians(VP->Roll));

	if (NavVector.z > 0.3) return;

	NavVector = (*modelMat)*  NavVector;

	draw->BindTex2D(Tex);
	float alpha = NavVector.z > 0 ? 0.3f : 1.f;

	draw->SetDrawColor2D(1, 1, 1, alpha);
	draw->DrawRect2D(NavVector.x - 30, NavVector.y - 30, NavVector.x + 30, NavVector.y + 30);

}