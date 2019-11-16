#include "Dock.h"
#include <glm/gtc/matrix_transform.hpp>

Dock::Dock(WidgetStuff ws) : NavBall(ws) {

}
void Dock::LoadNavBallTextures() {

}
void Dock::Tick(Draw* draw) {
	WindowUpdate(draw);

	VesselPacket VP = client->Vessel;

	float rad = (std::min(size.x, size.y) - 50) / 2;

	//render the ball
	glm::mat4 modelMat = glm::mat4(1);
	modelMat = glm::translate(modelMat, glm::vec3(pos.x + size.x / 2, pos.y + size.y / 2, 1));
	modelMat = glm::scale(modelMat, glm::vec3(rad / BORDER_SCALE, rad / BORDER_SCALE, rad / BORDER_SCALE));

	glm::mat4 rotMat = glm::mat4(1);
	rotMat = glm::rotate(rotMat, glm::radians(-VP.Heading + 90), glm::vec3(0, 0, 1));
	rotMat = glm::rotate(rotMat, glm::radians(VP.Pitch - 90), glm::vec3(1, 0, 0));
	rotMat = rotMat * glm::rotate(glm::mat4(1), glm::radians(-VP.Roll), glm::vec3(0, 0, 1));

	NavBall::SD->DrawSphere(draw, 0, modelMat, rotMat);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(1, 1, 1);
	draw->SetDrawColor2D(0, 0, 1, 0.5); //DRAW CROSSHAIRS
	draw->DrawRect2D(pos.x, pos.y + size.y / 2 - 1, pos.x + size.x, pos.y + size.y / 2 + 1);
	draw->SetDrawColor2D(1, 0, 0, 0.5);
	draw->DrawRect2D(pos.x + size.x / 2 - 1, pos.y, pos.x + size.x / 2 + 1, pos.y + size.y);

	if (client->Vessel.HasTarget) {
		char buff[32];

		renderNavHeading(VP.Target, &VP, draw, &modelMat, TL->SASModeTex(SAS_TARGET));
		renderNavHeading(NavHeading(-VP.Target.Pitch, VP.Target.Heading + 180), &VP, draw, &modelMat, TL->SASModeTex(SAS_ANTITARGET));
		renderNavHeading(NavHeading(-VP.TargetRotation.Pitch, VP.TargetRotation.Heading + 180), &VP, draw, &modelMat, TL->SASModeTex(SAS_PROGRADE));
		draw->BindTextShader();
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "Target: " + client->OrbitPlan.TargetName, pos.x + 5, pos.y + 15);
		sprintf(buff, "Rel. V: %.1f m/s", client->Vessel.TargetV);
		draw->DrawString(f, buff, pos.x + 5, pos.y + 30);
		sprintf(buff, "Dist.: %.1f m", client->Vessel.TargetDist);
		draw->DrawString(f, buff, pos.x + 5, pos.y + 45);
	}
	else {
		draw->BindTextShader();
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "No Target", pos.x + (size.x - f->GetTextWidth("No Target")) / 2, pos.y + size.y/2);
	}
}
