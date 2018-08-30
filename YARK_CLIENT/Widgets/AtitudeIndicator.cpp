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

#define TEX_Y_OFFSET 6
//#define TEX_LEFT_X_OFFSET 10
#define DOWN_SHIFT 5

#include "../Reg.h"

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
	view = glm::translate(view, glm::vec3(0, VP.Pitch * YSPACE + DOWN_SHIFT, 0));

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
	draw->DrawString(f, text, pos.x + size.x / 2 - 85 - f->GetTextWidth(text), pos.y + size.y / 2 + 4 + DOWN_SHIFT);//DRAW PITCH ANGLES BY CHEVRON
	draw->DrawString(f, text, pos.x + size.x / 2 + 85, pos.y + size.y / 2 + 4 + DOWN_SHIFT);

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

	int ARC_OFFSET = RegInt("ATI-IN_ARC_OFFSET", 65);

	view = glm::translate(glm::mat4(1), glm::vec3(pos.x + size.x / 2, pos.y + size.x / 2, 0));
	view = glm::rotate(view, glm::radians(VP.Roll), glm::vec3(0, 0, 1));
	view = glm::translate(view, -glm::vec3(pos.x + size.x / 2, pos.y + size.x / 2, 0));
	draw->draw2DShader->SetView(view);
#define TRI_TEX_OFFSET 20
	//draw->draw2DShader->SetView(glm::mat4(1));
	draw->DrawRect2D(pos.x + size.x / 2 - 8, pos.y + TRI_TEX_OFFSET , pos.x + size.x / 2 + 8, pos.y + TRI_TEX_OFFSET - 16); //DRAW TRIANGLE OVER ARC
	draw->draw2DShader->SetView(glm::mat4(1));
	

	draw->SetDrawColor2D(1, 1, 1); //DRAW CHEVRON
	draw->BindTex2D(chevronTex);
	draw->DrawRectUV2D(pos.x + size.x / 2 - 85, pos.y + size.y / 2 - 1 + DOWN_SHIFT, pos.x + size.x / 2 + 85, pos.y + size.y / 2 + 20 + DOWN_SHIFT, 0.f / 256.f, 0.f / 256.f, 239.f / 256.f, 40.f / 256.f);

	glLineWidth(2); //DRAW ARC
	draw->BindTex2D(0);
	draw->SetDrawColor2D(0, 1, 0);

	for (float theta = -90.f; theta <= 90.f; theta += 5.f) {

		float y = -glm::cos(glm::radians(theta));
		if (VP.Roll > 90.f || VP.Roll < -90.f) {
			y = -y;
		}
		float x = glm::sin(glm::radians(theta));

		float r1 = size.x / 2 - ARC_OFFSET, r2;
		if ((int)theta % 10) {
			r2 = size.x / 2 - ARC_OFFSET - 10;
		}
		else {
			r2 = size.x / 2 - ARC_OFFSET - 20;
		}
		float y_down = size.x / 2 - ARC_OFFSET + 25;
		draw->DrawLine2D(pos.x + size.x / 2 + x * r1, pos.y + y_down + y * r1, pos.x + size.x / 2 + x * r2, pos.y + y_down + y * r2);
	}

	int BAR_INSET = RegInt("ATI-IN_BARSINSET", 50);
	int VELO_YSPACE = RegInt("ATI-IN_VELO_YSPACE", 35);
	int VELO_DELTA = RegInt("ATI-IN_VELO_DELTA", 10);

	float height = (pos.x + size.y - 10) - (pos.y + 10);

	//DRAW VELOCITY BAR
	int y_mod = int(VP.Vsurf / VELO_DELTA * VELO_YSPACE) % VELO_YSPACE;
	int y_down = int(VP.Vsurf) / VELO_DELTA * VELO_DELTA;
	draw->DrawLine2D(pos.x + BAR_INSET, pos.y + 10, pos.x + BAR_INSET, pos.x + size.y - 10);
	for (int i = -height / 2 / VELO_YSPACE; i <= height / 2 / VELO_YSPACE; i++) {
		draw->DrawLine2D(pos.x + BAR_INSET, pos.y + size.y / 2 - i * VELO_YSPACE + y_mod, pos.x + BAR_INSET - 10, pos.y + size.y / 2 - i * VELO_YSPACE + y_mod);
		//draw > DrawString();

	}
	draw->BindTex2D(tri_90);
	draw->DrawRectUV2D(pos.x + BAR_INSET + 2, pos.y + size.y / 2 - 8, pos.x + BAR_INSET + 2 + 16, pos.y + size.y / 2 + 8, 1, 0, 0, 1);
	draw->BindTex2D();
	draw->BindTextShader();
	for (int i = -height / 2 / VELO_YSPACE; i <= height / 2 / VELO_YSPACE; i++) {
		std::string s = std::to_string((i * VELO_DELTA + y_down));
		draw->DrawString(f, s, pos.x + BAR_INSET - 10 - f->GetTextWidth(s), pos.y + size.y / 2 - i * VELO_YSPACE + TEX_Y_OFFSET + y_mod);
	}




	draw->BindDraw2DShader(); //DRAW ALTITUDE BAR

	int ALT_YSPACE = RegInt("ATI-IN_ALT_YSPACE", 35);
	int ALT_DELTA = RegInt("ATI-IN_ALT_DELTA", 10);

	y_mod = int(VP.RAlt / ALT_DELTA * ALT_YSPACE) % ALT_YSPACE;
	y_down = int(VP.RAlt) / ALT_DELTA * ALT_DELTA;
	draw->DrawLine2D(pos.x + size.x - BAR_INSET, pos.y + 10, pos.x + size.x - BAR_INSET, pos.x + size.y + 10);
	for (int i = -height / 2 / ALT_YSPACE; i <= height / 2 / ALT_YSPACE; i++) {
		draw->DrawLine2D(pos.x + size.x - BAR_INSET, pos.y + size.y / 2 - i * ALT_YSPACE + y_mod, pos.x + size.x - BAR_INSET + 10, pos.y + size.y / 2 - i * ALT_YSPACE + y_mod);
	}
	draw->BindTex2D(tri_90);
	draw->DrawRect2D(pos.x + size.x - BAR_INSET - 16 - 2, pos.y + size.y / 2 - 8, pos.x + size.x - BAR_INSET - 2, pos.y + size.y / 2 + 8);
	draw->BindTex2D();
	draw->BindTextShader();
	for (int i = -height / 2 / ALT_YSPACE; i <= height / 2 / ALT_YSPACE; i++) {
		std::string s = std::to_string((i * ALT_DELTA + y_down));
		draw->DrawString(f, s, pos.x + size.x - BAR_INSET + 10, pos.y + size.y / 2 - i * ALT_YSPACE + TEX_Y_OFFSET + y_mod);
	}

}
