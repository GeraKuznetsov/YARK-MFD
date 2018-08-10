#include "AtitudeIndicator.h"

#include "NavBall.h"
#include <iostream>

AtitudeIndicator::AtitudeIndicator(WidgetStuff ws) : Widget(ws) {
	this->client = client;
	chevronTex = loadTexture("Tex/ai/chevron.png");
	triTex = loadTexture("Tex/ai/triangle.png");
	tri_90 = loadTexture("Tex/ai/tri_90.png");
}

//#define GLM_ENABLE_EXPERIMENTAL
#include "gtc\matrix_transform.hpp"

#define XSPACE 35
#define YSPACE 8
#define XINSET 5

#define TEX_Y_OFFSET 8
//#define TEX_LEFT_X_OFFSET 10

void AtitudeIndicator::Tick(Draw* draw) {
	WindowUpdate(draw);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	VesselPacket VP = (*client) ? (*client)->vesselPacket : VesselPacket();
	//VP.Heading = 0;
	float rad = (size.x - 100) / 2;

	glm::mat4 	view = glm::translate(glm::mat4(1), glm::vec3(pos.x + size.x / 2, pos.y + size.y / 2, 0));
	view = glm::rotate(view, glm::radians(VP.Roll), glm::vec3(0, 0, 1));
	glm::mat4 viewNoPitch = view;
	view = glm::translate(view, glm::vec3(0, VP.Pitch * YSPACE, 0));

	draw->draw2DShader->SetView(view);

	draw->SetDrawColor2D(29.f / 256.f, 90.f / 256.f, 234.f / 256.f); //DRAW HORIZON
	draw->DrawRect2D(-size.x, -90 * YSPACE, size.x * 2, 0);
	draw->SetDrawColor2D(140.f / 256.f, 80.f / 256.f, 1.f / 256.f);
	draw->DrawRect2D(-size.x, 0, size.x * 2, 90 * YSPACE);

	glLineWidth(2);
	draw->SetDrawColor2D(0, 1, 0);
	for (int y = -90; y <= 90; y += 5) {//DRAW PITCH LINES
		if (!(y % 2)) {
			draw->DrawLine2D(-XSPACE, (y)* YSPACE, XSPACE, (y)* YSPACE);
		}
		else {
			draw->DrawLine2D(-XSPACE + XINSET, (y)* YSPACE, XSPACE - XINSET, (y)* YSPACE);
		}
	}

	draw->BindTextShader();
	draw->SetDrawColor2D(0, 1, 0);
	draw->textShader->SetView(view);
	for (int y = -90; y <= 90; y += 5) { //DRAW PITCH ANGLES
		if (!(y % 2)) {
			draw->DrawString(f, std::to_string(-y), -XSPACE - f->GetTextWidth(std::to_string(-y)), (y)* YSPACE + TEX_Y_OFFSET);
			draw->DrawString(f, std::to_string(-y), XSPACE, (y)* YSPACE + TEX_Y_OFFSET);
		}
	}
	draw->textShader->SetView(glm::mat4(1));
	std::string text = std::to_string((int)VP.Pitch);
	draw->DrawString(f, text, pos.x + size.x / 2 - 85 - f->GetTextWidth(text), pos.y + size.y / 2 + TEX_Y_OFFSET);//DRAW PITCH ANGLES BY CHEVRON
	draw->DrawString(f, text, pos.x + size.x / 2 + 85, pos.y + size.y / 2 + TEX_Y_OFFSET);

	int roll = abs((int)VP.Roll);
	if (roll > 90) {
		roll = 180 - roll;
	}
	text = std::to_string(roll);
	draw->SetTextColor(0.6, 1, 0.2);
	draw->DrawString(f, text, pos.x + size.x / 2 - f->GetTextWidth(text) / 2, pos.y + 60);//DRAW ROLL ANGLE UNDER ARC


	draw->BindDraw2DShader();

	glLineWidth(3);
	draw->SetDrawColor2D(0.5, 0.5, 0);
	draw->DrawLine2D(-XSPACE, -VP.Prograde.Pitch*YSPACE - 5, -XSPACE, -VP.Prograde.Pitch*YSPACE + 5);
	draw->DrawLine2D(XSPACE, -VP.Prograde.Pitch*YSPACE - 5, XSPACE, -VP.Prograde.Pitch*YSPACE + 5);
	draw->SetDrawColor2D(1, 0.8, 0);
	draw->DrawLine2D(-XSPACE, -VP.Prograde.Pitch*YSPACE, XSPACE, -VP.Prograde.Pitch*YSPACE);//DRAW PROGRADE BAR

	float headingOffset = VP.Heading - VP.Prograde.Heading;
	if (headingOffset < -180) {
		headingOffset += 360;
	}if (headingOffset > 180) {
		headingOffset -= 360;
	}
	draw->DrawLine2D(-XSPACE + headingOffset, -VP.Prograde.Pitch*YSPACE - 5, -XSPACE + headingOffset, -VP.Prograde.Pitch*YSPACE + 5);
	draw->DrawLine2D(XSPACE + headingOffset, -VP.Prograde.Pitch*YSPACE - 5, XSPACE + headingOffset, -VP.Prograde.Pitch*YSPACE + 5);

	draw->SetDrawColor2D(1, 1, 1);
	draw->BindTex2D(triTex);
	draw->draw2DShader->SetView(viewNoPitch);
#define TRI_TEX_OFFSET 16
	draw->DrawRect2D(-8, -size.y / 2 + TRI_TEX_OFFSET, 8, -size.y / 2 - 16 + TRI_TEX_OFFSET); //DRAW TRIANGLE OVER ARC
	draw->draw2DShader->SetView(glm::mat4(1));



	draw->SetDrawColor2D(1, 1, 1); //DRAW CHEVRON
	draw->BindTex2D(chevronTex);
	draw->DrawRectUV2D(pos.x + size.x / 2 - 85, pos.y + size.y / 2 - 1, pos.x + size.x / 2 + 85, pos.y + size.y / 2 + 20, 0.f / 256.f, 0.f / 256.f, 239.f / 256.f, 40.f / 256.f);

	glLineWidth(2); //DRAW ARC
	draw->BindTex2D(0);
	draw->SetDrawColor2D(0, 1, 0);
	for (float theta = -90.f; theta <= 90.f; theta += 5.f) {

		float y = -glm::cos(glm::radians(theta));
		if (VP.Roll > 90.f || VP.Roll < -90.f) {
			y = -y;
		}
		float x = glm::sin(glm::radians(theta));

		float r1 = size.x / 2 - 25, r2;
		if ((int)theta % 10) {
			r2 = size.x / 2 - 35;
		}
		else {
			r2 = size.x / 2 - 45;
		}

		draw->DrawLine2D(pos.x + size.x / 2 + x * r1, pos.y + size.y / 2 + y * r1, pos.x + size.x / 2 + x * r2, pos.y + size.y / 2 + y * r2);
	}

}
