#include "Dock.h"
#include <glm/gtc/matrix_transform.hpp>

#undef min
#undef max

#if _WIN32
#define sprintf sprintf_s
#endif

Dock::Dock() {

}

std::string Dock::GetTitle() {
	return "Docking Alignment";
}

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

glm::vec3 CalcVector(float heading, float pitch) {
	renderCoords rc;
	float tempHeading = glm::radians(heading);
	float tempPitch = glm::radians(pitch);
	return glm::vec4(glm::cos(tempPitch) * glm::sin(tempHeading), -glm::sin(tempPitch), -glm::cos(tempPitch) * glm::cos(tempHeading), 1);
	//return glm::rotateZ(NavVector, glm::radians(roll));
}

glm::vec3 CalcV(NavHeading NH, VesselPacket* VP) {
	float tempHeading = glm::radians(NH.Heading);
	float tempPitch = glm::radians(NH.Pitch);
	glm::vec4 NavVector = glm::vec4(glm::cos(tempPitch) * glm::sin(tempHeading), -glm::sin(tempPitch), -glm::cos(tempPitch) * glm::cos(tempHeading), 1);
	NavVector = glm::rotateY(NavVector, glm::radians(VP->Heading));
	NavVector = glm::rotateX(NavVector, glm::radians(VP->Pitch));
	NavVector = glm::rotateZ(NavVector, glm::radians(VP->Roll));
	return NavVector;
}

#include <glm/gtx/vector_angle.hpp>

#define INSET 10
#define SYM_SIZE 40

void Dock::Draw(XY pos, XY size) {
	glLineWidth(2);
	VesselPacket VP = client.Vessel;

	float rad = (glm::min(size.x, size.y) - 50) / 2;

	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(1, 1, 1);
	draw->BindTex2D();
	int minSize = glm::min(size.x, size.y) - 50;
	int x1 = pos.x + (size.x - minSize) / 2;
	int y1 = pos.y + (size.y - minSize) / 2;
	int x2 = pos.x + (size.x + minSize) / 2;
	int y2 = pos.y + (size.y + minSize) / 2;
	int xc = pos.x + (size.x) / 2;
	int yc = pos.y + (size.y) / 2;

	draw->DrawLine2D(xc, y1, xc, y2);
	draw->DrawLine2D(x1, yc, x2, yc);

	if (client.Vessel.HasTarget) {
		char buff[32];

		glm::vec3 ToTarget = -CalcV(VP.Target, &VP) * VP.TargetDist;
		glm::vec3 TVelocity = CalcV(VP.Prograde, &VP) * VP.TargetV;
		glm::vec3 TargetRot = CalcV(VP.TargetRotation, &VP);

		float angleX = glm::degrees(glm::atan(TargetRot.x, TargetRot.z));
		float angleY = glm::degrees(glm::asin(TargetRot.y));
		float angle = 180 - glm::degrees(glm::angle(CalcV(NavHeading(VP.Pitch, VP.Heading), &VP), TargetRot));

		draw->SwitchShader(SHADER_2D);
		draw->BindTex2D(TL.SASModeTex(14));
		float angX = angleX;
		if (angX > 20) {
			angX = 20;
		}
		if (angX < -20) {
			angX = -20;
		}
		float angY = angleY;
		if (angY > 20) {
			angY = 20;
		}
		if (angY < -20) {
			angY = -20;
		}
		draw->DrawRect2D(xc + angX / 20 * minSize / 2 - SYM_SIZE, yc + -angY / 20 * minSize / 2 - SYM_SIZE, xc + angX / 20 * minSize / 2 + SYM_SIZE, yc + -angY / 20 * minSize / 2 + SYM_SIZE);
		draw->BindTex2D(TL.SASModeTex(13));
		float offsetX = ToTarget.x / abs(ToTarget.z) * 10;
		if (offsetX > 20) {
			offsetX = 20;
		}
		if (offsetX < -20) {
			offsetX = -20;
		}
		float offsetY = ToTarget.y / abs(ToTarget.z) * 10;
		if (offsetY > 20) {
			offsetY = 20;
		}
		if (offsetY < -20) {
			offsetY = -20;
		}
		draw->DrawRect2D(xc + offsetX / 20 * minSize / 2 - SYM_SIZE, yc + -offsetY / 20 * minSize / 2 - SYM_SIZE, xc + offsetX / 20 * minSize / 2 + SYM_SIZE, yc + -offsetY / 20 * minSize / 2 + SYM_SIZE);\
		draw->BindTex2D();
		float px = x1;
		float dpx = float(minSize) / 10.f;
		for (int i = -5; i <= 5; i++) {
			draw->DrawLine2D(px, yc - 5, px, yc + 5);
			px += dpx;
		}

		float py = y1;
		for (int i = -5; i <= 5; i++) {
			draw->DrawLine2D(xc - 5, py, xc + 5, py);
			py += dpx;
		}

		draw->SwitchShader(SHADER_TEXT);
		draw->SetTextColor(1, 1, 1);
		int y = pos.y + 15;
		draw->DrawString(f, "Target: " + client.orbitPlan.TargetName, pos.x + 5, pos.y + 15); y += 15;
		sprintf(buff, "Dist.: %.2f m", client.Vessel.TargetDist);
		draw->DrawString(f, buff, pos.x + 5, y); y += 15;
		sprintf(buff, "X.: %.2f m", ToTarget.x);
		draw->DrawString(f, buff, pos.x + 5, y); y += 15;
		sprintf(buff, "Y.: %.2f m", ToTarget.y);
		draw->DrawString(f, buff, pos.x + 5, y); y += 15;
		sprintf(buff, "Z.: %.2f m", ToTarget.z);
		draw->DrawString(f, buff, pos.x + 5, y); y += 15;

		sprintf(buff, "Vel: %.1f m/s", client.Vessel.TargetV);
		draw->DrawString(f, buff, pos.x + 5, y); y += 15;
		sprintf(buff, "dX: %.1f m/s", TVelocity.x);
		draw->DrawString(f, buff, pos.x + 5, y); y += 15;
		sprintf(buff, "dY: %.1f m/s", TVelocity.y);
		draw->DrawString(f, buff, pos.x + 5, y); y += 15;
		sprintf(buff, "DZ: %.1f m/s", TVelocity.z);
		draw->DrawString(f, buff, pos.x + 5, y); y += 15;

		y = pos.y + 15;
		int	x = pos.x + size.x - 170;
		sprintf(buff, "Angle.: %d deg", (int)angle);
		draw->DrawString(f, buff, x, y); y += 15;
		sprintf(buff, "Yaw.: %d deg", (int)angleX);
		draw->DrawString(f, buff, x, y); y += 15;
		sprintf(buff, "Pitch.: %d deg", (int)angleY);
		draw->DrawString(f, buff, x, y); y += 15;

		if (killV) {
			client.Control.SetControlerMode(CONTROLLER_TRANS, AXIS_INT_NZ);
			float TX =  (TVelocity.x > 0 ? 1 : -1) * glm::min(1.f, abs(TVelocity.x));
			float TY =  (TVelocity.y > 0 ? 1 : -1) * glm::min(1.f, abs(TVelocity.y));
			client.Control.InputTran(TX * 1000, TY * 1000, 0);

			draw->SetTextColor(0, 1, 0);
			draw->DrawString(f, "[X] - Disable Kill Lat. Velocity", pos.x + size.x - 220, pos.y + size.y - 10);
			
		}
		else {
			draw->SetTextColor(1, 0, 0);
			draw->DrawString(f, "[X] - Enable Kill Lat. Velocity", pos.x + size.x - 220, pos.y + size.y - 10);
		
		}
		if (win->KeyTyped(SDL_SCANCODE_X)) {
			killV = !killV;
		}

		if (ToTarget.z < 0) {
			draw->SetTextColor(1, 0, 0);
			draw->DrawString(f, "Warn: Behind Target", pos.x +5, pos.y + size.y - 10);
		}

	}
	else {
		draw->SwitchShader(SHADER_TEXT);
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "No Target", pos.x + pos.y + 20, pos.y + 15);
}
}
