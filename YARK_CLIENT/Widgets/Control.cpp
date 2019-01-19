#include "Control.h"
#include "Util\IM.h"
#include "../Reg.h"

Control::Control(WidgetStuff ws) : Widget(ws) {
	push = loadTexture("Tex/push.png", false);
}

void Control::Toggle(std::string option, bool* ptr, Draw* draw) {
	XY renderAt = lastPos + this->pos;
	IM::ToggleSwitch(renderAt, win, draw, option, ptr);
	lastPos.x += TOGGLE_SIZE + 20;
}

void Control::SASOption(Draw* draw, uint8_t thisMode,Client* c) {
	XY renderAt = lastPos + this->pos;
	IM::LED(XY{ renderAt.x + 17, renderAt.y }, win, draw, c->vesselPacket.SASMode == thisMode);
	draw->BindTex2D(push);
	renderAt += XY{ 0,38 };
	draw->DrawRect2D(renderAt.x, renderAt.y, renderAt.x + 64, renderAt.y + 64);
	draw->BindTex2D(TL->SASModeTex(thisMode));
	int inset = 10;
	draw->DrawRect2D(renderAt.x + inset, renderAt.y + inset, renderAt.x + 64 - inset, renderAt.y + 64 - inset);

	renderAt += XY{ 32,32 };
	if ((win->MouseX() - renderAt.x)*(win->MouseX() - renderAt.x) + (win->MouseY() - renderAt.y)*(win->MouseY() - renderAt.y) < 32 * 32) {
		if (win->MouseClicked(SDL_BUTTON_LEFT)) {
			c->controlPacket.SASMode = thisMode;
		}
	}
	lastPos.x += TOGGLE_SIZE + 20;
}

void Control::Tick(Draw* draw) {
	WindowUpdate(draw);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(62.f / 256.f, 62.f / 256.f, 61.f / 256.f);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	lastPos = XY{ 16,0 };
	Toggle("Brakes", &brakes, draw);
	Toggle("Lights", &lights, draw);
	Toggle("Gear", &gear, draw);
	Toggle("SAS", &sas, draw);
	Toggle("RCS", &rcs, draw);
	client->SetMainControl(MC_SAS, sas);
	client->SetMainControl(MC_RCS, rcs);
	client->SetMainControl(MC_BRAKES, brakes);
	client->SetMainControl(MC_LIGHTS, lights);
	client->SetMainControl(MC_GEAR, gear);

	lastPos = XY{ 16,130 };
	Client* ptr = client;
	SASOption(draw, SAS_HOLD, ptr);
	SASOption(draw, SAS_PROGRADE, ptr);
	SASOption(draw, SAS_NORMAL, ptr);
	SASOption(draw, SAS_RADIN, ptr);
	SASOption(draw, SAS_TARGET, ptr);

	lastPos.x = 16;
	lastPos.y += 120;

	SASOption(draw, SAS_MAN, ptr);
	SASOption(draw, SAS_RETROGRADE, ptr);
	SASOption(draw, SAS_ANTINORMAL, ptr);
	SASOption(draw, SAS_RADOUT, ptr);
	SASOption(draw, SAS_ANTITARGET, ptr);


}
