#include "NavBall.h"
#include <iostream>
#include "../Reg.h"
SphereDraw *NavBall::SD;

SphereDraw::SphereDraw() {
	shader = LoadSHader("Shaders/sphere.vert", "Shaders/sphere.frag");
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
}

NavBall::NavBall() {
	if (!NavBall::SD) {
		NavBall::SD = new SphereDraw();
	}
	LoadNavBallTextures();
}

#define GLM_ENABLE_EXPERIMENTAL
#include "gtc\matrix_transform.hpp"
#include "gtx/rotate_vector.hpp"

std::string NavBall::GetTitle() {
	return "NavBall";
}
void SphereDraw::DrawSphere(GLuint texture, glm::mat4 modelMat, glm::mat4 rotMat) {
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

renderCoords NavBall::calcRenderCoords(NavHeading NH, VesselPacket *VP, glm::mat4 *modelMat) {
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

void NavBall::renderNavHeading(NavHeading NH, VesselPacket *VP,  glm::mat4 *modelMat, GLuint Tex) {
	renderCoords rc = calcRenderCoords(NH, VP, modelMat);
	draw->BindTex2D(Tex);
	draw->SetDrawColor2D(1, 1, 1, rc.alpha);
	draw->DrawRect2D(rc.x - 30, rc.y - 30, rc.x + 30, rc.y + 30);
}

void NavBall::DrawRect(int x, int y, int width, int height, XY pos) {
	draw->DrawLine2D(pos.x + x - width, pos.y + y - height, pos.x + x + width, pos.y + y - height);
	draw->DrawLine2D(pos.x + x + width, pos.y + y - height, pos.x + x + width, pos.y + y + height);
	draw->DrawLine2D(pos.x + x - width, pos.y + y - height, pos.x + x - width, pos.y + y + height);
	draw->DrawLine2D(pos.x + x - width, pos.y + y + height, pos.x + x + width, pos.y + y + height);
}

void SetTCol(bool b) {
	if (b) {
		draw->SetTextColor(0, 1, 0);
	}
	else {
		draw->SetTextColor(1, 0, 0);
	}
}

void NavBall::Draw(XY pos, XY size) {
	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	VesselPacket VP = client.Vessel;

	float rad = (min(size.x, size.y) - 130) / 2;

	//render the ball
	glm::mat4 modelMat = glm::mat4(1);
	modelMat = glm::translate(modelMat, glm::vec3(pos.x + size.x / 2, pos.y + size.y / 2, 1));
	modelMat = glm::scale(modelMat, glm::vec3(rad / BORDER_SCALE, rad / BORDER_SCALE, rad / BORDER_SCALE));

	glm::mat4 rotMat = glm::mat4(1);
	rotMat = glm::rotate(rotMat, glm::radians(-VP.Heading + 90), glm::vec3(0, 0, 1));
	rotMat = glm::rotate(rotMat, glm::radians(VP.Pitch - 90), glm::vec3(1, 0, 0));
	rotMat = rotMat * glm::rotate(glm::mat4(1), glm::radians(-VP.Roll), glm::vec3(0, 0, 1));

	NavBall::SD->DrawSphere(  navballTex, modelMat, rotMat);

	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(1, 1, 1);

	draw->BindTex2D(chevron);  //DRAW CHEVRON
	draw->DrawRectUV2D(pos.x + size.x / 2 - 90, pos.y + size.y / 2 - 35, pos.x + size.x / 2 + 90, pos.y + size.y / 2 + 35, 0.f / 256.f, 0.f / 256.f, 180.f / 256.f, 70.f / 256.f);
	
	renderNavHeading(VP.Prograde, &VP,   &modelMat, TL.SASModeTex(SAS_PROGRADE));
	renderNavHeading(NavHeading(-VP.Prograde.Pitch, VP.Prograde.Heading + 180), &VP,   &modelMat, TL.SASModeTex(SAS_RETROGRADE));

	if (VP.SpeedMode == 1) {
		if (VP.Prograde.Pitch >= 0) {
			renderNavHeading(NavHeading(90 - VP.Prograde.Pitch, VP.Prograde.Heading + 180), &VP,   &modelMat, TL.SASModeTex(SAS_RADIN));
			renderNavHeading(NavHeading(VP.Prograde.Pitch - 90, VP.Prograde.Heading), &VP,   &modelMat, TL.SASModeTex(SAS_RADOUT));
		}
		else {
			renderNavHeading(NavHeading(VP.Prograde.Pitch + 90, VP.Prograde.Heading), &VP,   &modelMat, TL.SASModeTex(SAS_RADOUT));
			renderNavHeading(NavHeading(-VP.Prograde.Pitch - 90, VP.Prograde.Heading + 180), &VP,   &modelMat, TL.SASModeTex(SAS_RADIN));
		}

		renderNavHeading(NavHeading(0.f, VP.Prograde.Heading - 90), &VP,   &modelMat, TL.SASModeTex(SAS_NORMAL));
		renderNavHeading(NavHeading(0.f, VP.Prograde.Heading + 90), &VP,   &modelMat, TL.SASModeTex(SAS_ANTINORMAL));
	}

	if (VP.MNDeltaV) {
		renderNavHeading(VP.Maneuver, &VP,   &modelMat, TL.SASModeTex(SAS_MAN));
	}
	if (VP.HasTarget) {
		renderNavHeading(VP.Target, &VP,   &modelMat, TL.SASModeTex(SAS_TARGET));
		renderNavHeading(NavHeading(-VP.Target.Pitch, VP.Target.Heading + 180), &VP,   &modelMat, TL.SASModeTex(SAS_ANTITARGET));
	}
	if (client.Control.SASMode == SAS_HOLD_VECTOR) {
		renderCoords rc = calcRenderCoords(NavHeading(Registry["SASS_PITCH"], Registry["SASS_HEADING"]), &VP, &modelMat);
		if (rc.alpha < 0.1)rc.alpha = 0.1;
		draw->SetDrawColor2D(1, 1, 1, rc.alpha);
		draw->BindTex2D(TL.SASModeTex(SAS_HOLD_VECTOR));
		draw->DrawRect2D(rc.x - 30, rc.y - 30, rc.x + 30, rc.y + 30);
		draw->BindTex2D();
		glLineWidth(3);
		draw->SetDrawColor2D(215.f / 256.f, 254 / 256.f, 0, rc.alpha);
		draw->DrawLine2D(rc.x, rc.y, rc.x + glm::sin(glm::radians(Registry["SASS_ROLL"] + 180 - VP.Roll)) * 20, rc.y + glm::cos(glm::radians(Registry["SASS_ROLL"] + 180 - VP.Roll)) * 20);
	}

	//render the flair
	char buff[16];

	Font *font = f;

	draw->SwitchShader(SHADER_TEXT);
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(font, "RADAR ALT", pos.x + (size.x - font->GetTextWidth("RADAR ALT")) / 2, pos.y + size.y - 30);
	sprintf_s(buff, "%.2f m", client.Vessel.RAlt);
	draw->DrawString(font, buff, pos.x + (size.x - font->GetTextWidth(buff)) / 2, pos.y + size.y - 10);

	int inset = 75;
	draw->DrawString(font, "HOR SPEED", pos.x + (inset - font->GetTextWidth("HOR SPEED") / 2), pos.y + size.y - 30);
	sprintf_s(buff, "%.2f m/s", sqrt(pow(client.Vessel.Vsurf, 2) - pow(client.Vessel.VVI, 2)));
	draw->DrawString(font, buff, pos.x + (inset - font->GetTextWidth(buff) / 2), pos.y + size.y - 10);

	draw->DrawString(font, "VERT SPEED", pos.x + (size.x - inset - font->GetTextWidth("VERT SPEED") / 2), pos.y + size.y - 30);
	sprintf_s(buff, "%.2f m/s", client.Vessel.VVI);
	draw->DrawString(font, buff, pos.x + (size.x - inset - font->GetTextWidth(buff) / 2), pos.y + size.y - 10);

	draw->DrawString(font, "HEADING", pos.x + (size.x - font->GetTextWidth("HEADING")) / 2, pos.y + 30);
	sprintf_s(buff, "%.1f", client.Vessel.Heading);
	draw->DrawString(font, buff, pos.x + (size.x - font->GetTextWidth(buff)) / 2, pos.y + 50);

	std::string speedString = "SPEED (";
	float speed = 0;
	std::string mode;
	switch (client.Vessel.SpeedMode) {
	case 1:
		speedString += "ORB)";
		speed = client.Vessel.VOrbit;
		break;
	case 2:
		speedString += "SRF)";
		speed = client.Vessel.Vsurf;
		break;
	case 3:
		speedString += "TRG)";
		speed = client.Vessel.TargetV;
		break;
	default:
		speedString += "ORB)";
		speed = client.Vessel.VOrbit;
	}


	draw->DrawString(font, speedString, pos.x + (inset - font->GetTextWidth(speedString) / 2), pos.y + 30);
	sprintf_s(buff, "%.1f m/s", speed);
	draw->DrawString(font, buff, pos.x + (inset - font->GetTextWidth(buff) / 2), pos.y + 50);

	draw->DrawString(font, "SAS MODE", pos.x + (size.x - inset - font->GetTextWidth("SAS MODE") / 2), pos.y + 30);
	if (client.Vessel.GetMainControl(MC_SAS)) {
		draw->SwitchShader(SHADER_2D);
		draw->BindTex2D(TL.SASModeTex(client.Vessel.SASMode));
		int width = 20;
		draw->SetDrawColor2D(1, 1, 1);
		draw->DrawRect2D(pos.x + size.x - inset - width, pos.y + 55 - width, pos.x + size.x - inset + width, pos.y + 55 + width);
			draw->SwitchShader(SHADER_TEXT);
	}
	else {
		draw->DrawString(font, "OFF", pos.x + (size.x - inset - font->GetTextWidth("OFF") / 2), pos.y + 50);
	}

	inset = 50;
	draw->DrawString(font, "ROLL", pos.x + (size.x - inset - font->GetTextWidth("ROLL") / 2), pos.y + size.y / 4 * 3 - 10);
	sprintf_s(buff, "%.1f", client.Vessel.Roll);
	draw->DrawString(font, buff, pos.x + (size.x - inset - font->GetTextWidth(buff) / 2), pos.y + size.y / 4 * 3 + 10);

	draw->DrawString(font, "PITCH", pos.x + (size.x - inset - font->GetTextWidth("PITCH") / 2), pos.y + size.y / 4 - 10);
	sprintf_s(buff, "%.1f", client.Vessel.Pitch);
	draw->DrawString(font, buff, pos.x + (size.x - inset - font->GetTextWidth(buff) / 2), pos.y + size.y / 4 + 10);

	inset = 35;

	SetTCol(  client.Vessel.GetMainControl(MC_SAS));
	draw->DrawString(font, "SAS", pos.x + inset - font->GetTextWidth("RCS", -1, 0.95f) / 2, pos.y + size.y / 2 - 60, 0.95f);
	SetTCol(  client.Vessel.GetMainControl(MC_RCS));
	draw->DrawString(font, "RCS", pos.x + inset - font->GetTextWidth("RCS", -1, 0.95f) / 2, pos.y + size.y / 2 - 30, 0.95f);
	SetTCol(  client.Vessel.GetMainControl(MC_LIGHTS));
	draw->DrawString(font, "LGT", pos.x + inset - font->GetTextWidth("LGT", -1, 0.95f) / 2, pos.y + size.y / 2, 0.95f);
	SetTCol(  client.Vessel.GetMainControl(MC_GEAR));
	draw->DrawString(font, "LDG", pos.x + inset - font->GetTextWidth("LDG", -1, 0.95f) / 2, pos.y + size.y / 2 + 30, 0.95f);
	SetTCol(  client.Vessel.GetMainControl(MC_BRAKES));
	draw->DrawString(font, "BRK", pos.x + inset - font->GetTextWidth("BRK", -1, 0.95f) / 2, pos.y + size.y / 2 + 60, 0.95f);

	inset = 50;

	glLineWidth(3);
	int boxWidth = 40;
	int boxHeight = 20;
	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(0.9, 0.9, 0.9);
	draw->BindTex2D();
	DrawRect(size.x / 2, 35, boxWidth, boxHeight, pos);
	DrawRect(size.x - inset, size.y / 4 - 5, boxWidth, boxHeight, pos);
	DrawRect(size.x - inset, size.y / 4 * 3 - 5, boxWidth, boxHeight, pos);
}
