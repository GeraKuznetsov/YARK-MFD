#include "NavBall.h"
#include <iostream>

SphereDraw *NavBall::SD;

SphereDraw::SphereDraw() {
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
}

void NavBall::LoadNavBallTextures() {
	navballTex = loadTexture("Tex/navball/navball.png", false);
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
	SASSTex = loadTexture("Tex/navball/SASS.png");
}

NavBall::NavBall(WidgetStuff ws) : Widget(ws) {
	if (!NavBall::SD) {
		NavBall::SD = new SphereDraw();
	}
	LoadNavBallTextures();
}

#define GLM_ENABLE_EXPERIMENTAL
#include "gtc\matrix_transform.hpp"
#include "gtx/rotate_vector.hpp"

void SphereDraw::DrawSphere(Draw *draw, GLuint texture, glm::mat4 modelMat, glm::mat4 rotMat) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUseProgram(shader);
	SetShaderMat4(proj, draw->GetOrthroMat());

	SetShaderMat4(modelUnif, modelMat);

	SetShaderMat4(rotUnif, rotMat);

	glBindVertexArray(vao); //DRAW NAVBALL
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

struct renderCoords {
	float x, y, alpha;
};
renderCoords calcRenderCoords(NavHeading NH, VesselPacket *VP, glm::mat4 *modelMat) {
	renderCoords rc;
	float tempHeading = glm::radians(NH.Heading);
	float tempPitch = glm::radians(NH.Pitch);
	glm::vec4 NavVector = glm::vec4(glm::cos(tempPitch) * glm::sin(tempHeading), -glm::sin(tempPitch), -glm::cos(tempPitch) * glm::cos(tempHeading), 1);
	NavVector = glm::rotateY(NavVector, glm::radians(VP->Heading));
	NavVector = glm::rotateX(NavVector, glm::radians(VP->Pitch));
	NavVector = glm::rotateZ(NavVector, glm::radians(VP->Roll));

	NavVector = (*modelMat)*  NavVector;

	rc.x = NavVector.x;
	rc.y = NavVector.y;
	rc.alpha = NavVector.z > 0 ? 0.3f : 1.f;
	if (NavVector.z > 0.3) {
		rc.alpha = 0;
	}
	return rc;
}

void renderNavHeading(NavHeading NH, VesselPacket *VP, Draw* draw, glm::mat4 *modelMat, GLuint Tex) {
	renderCoords rc = calcRenderCoords(NH, VP, modelMat);
	draw->BindTex2D(Tex);
	draw->SetDrawColor2D(1, 1, 1, rc.alpha);
	draw->DrawRect2D(rc.x - 30, rc.y - 30, rc.x + 30, rc.y + 30);
}

#include "../Reg.h"
extern NavHeading SASS;
extern float SASS_roll;

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

	NavBall::SD->DrawSphere(draw, navballTex, modelMat, rotMat);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(1, 1, 1);

	draw->BindTex2D(chevron);  //DRAW CHEVRON
	draw->DrawRectUV2D(pos.x + size.x / 2 - 90, pos.y + size.y / 2 - 35, pos.x + size.x / 2 + 90, pos.y + size.y / 2 + 35, 0.f / 256.f, 0.f / 256.f, 180.f / 256.f, 70.f / 256.f);

	renderNavHeading(VP.Prograde, &VP, draw, &modelMat, PGTex);
	renderNavHeading(NavHeading(-VP.Prograde.Pitch, VP.Prograde.Heading + 180), &VP, draw, &modelMat, RGTex);

	if (VP.SpeedMode == 1) {
		if (VP.Prograde.Pitch >= 0) {
			renderNavHeading(NavHeading(90 - VP.Prograde.Pitch, VP.Prograde.Heading + 180), &VP, draw, &modelMat, ROTex);
			renderNavHeading(NavHeading(VP.Prograde.Pitch - 90, VP.Prograde.Heading), &VP, draw, &modelMat, RITex);
		}
		else {
			renderNavHeading(NavHeading(VP.Prograde.Pitch + 90, VP.Prograde.Heading), &VP, draw, &modelMat, ROTex);
			renderNavHeading(NavHeading(90 - VP.Prograde.Pitch, VP.Prograde.Heading + 180), &VP, draw, &modelMat, RITex);
		}

		renderNavHeading(NavHeading(0.f, VP.Prograde.Heading - 90), &VP, draw, &modelMat, NTex);
		renderNavHeading(NavHeading(0.f, VP.Prograde.Heading + 90), &VP, draw, &modelMat, ANTex);
	}

	if (VP.MNDeltaV) {
		renderNavHeading(VP.Maneuver, &VP, draw, &modelMat, MTex);
	}
	if (VP.HasTarget) {
		renderNavHeading(VP.Target, &VP, draw, &modelMat, TPGTex);
		renderNavHeading(NavHeading(-VP.Target.Pitch, VP.Target.Heading + 180), &VP, draw, &modelMat, TRGTex);
	}
	if (RegInt("FLYBYWIRE_SMART", 0)) {
		renderCoords rc = calcRenderCoords(SASS, &VP, &modelMat);
		if (rc.alpha < 0.1)rc.alpha = 0.1;
		draw->SetDrawColor2D(1, 1, 1, rc.alpha);
		draw->BindTex2D(SASSTex);
		draw->DrawRect2D(rc.x - 30, rc.y - 30, rc.x + 30, rc.y + 30);
		draw->BindTex2D();
		glLineWidth(3);
		draw->SetDrawColor2D(215.f / 256.f, 254 / 256.f, 0, rc.alpha);
		draw->DrawLine2D(rc.x, rc.y, rc.x + glm::sin(glm::radians(SASS_roll + 180 - VP.Roll)) * 20, rc.y + glm::cos(glm::radians(SASS_roll + 180 - VP.Roll)) * 20);
	}
}