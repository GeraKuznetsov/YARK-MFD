#include "Control.h"
#include "Util\IM.h"
#include "../Reg.h"

Control::Control(WidgetStuff ws) : Widget(ws) {
	sliderKnob = loadTexture("Tex/slider.png", false);
	memset((char*)&oldVessel, 0, sizeof(VesselPacket));
}

void Control::Toggle(std::string option, bool* ptr, Draw* draw) {
	XY renderAt = lastPos + this->pos;
	IM::ToggleSwitch(renderAt, win, draw, option, ptr);
	lastPos.x += TOGGLE_SIZE + 20;
}

void Control::SASOption(Draw* draw, uint8_t thisMode, Client* c) {
	XY renderAt = lastPos + this->pos;
	IM::LED(XY{ renderAt.x + 17, renderAt.y }, win, draw, c->Vessel.SASMode == thisMode);
	renderAt += XY{ 0,38 };
	if (IM::PushButton(draw, win, renderAt)) {
		c->Control.SASMode = thisMode;
	}
	//draw->BindTex2D(push);
	//draw->DrawRect2D(renderAt.x, renderAt.y, renderAt.x + 64, renderAt.y + 64);
	draw->BindTex2D(TL->SASModeTex(thisMode));
	int inset = 10;
	draw->DrawRect2D(renderAt.x + inset, renderAt.y + inset, renderAt.x + 64 - inset, renderAt.y + 64 - inset);

	lastPos.x += TOGGLE_SIZE + 20;
}
void Control::FloatPM(Draw* draw, std::string label, float* fl, float grad, int dig, bool sign) {
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, label, pos.x + lastPos.x - f->GetTextWidth(label) / 2, pos.y + lastPos.y);

	lastPos.y += 18;

	int width = (dig + sign) * 20;

	if (IM::Button(pos + lastPos + XY{ -width / 2 - 26,-7 }, win, draw, f, "-")) {
		(*fl) -= grad;
	}
	IM::SegInt(pos + lastPos + XY{ -width / 2,-12 }, win, draw, int(*fl), dig, sign);
	if (IM::Button(pos + lastPos + XY{ width / 2 - 0, -7 }, win, draw, f, "+")) {
		(*fl) += grad;
	}
	lastPos.y += 40;
}

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

void Control::Tick(Draw* draw) {
	WindowUpdate(draw);

	VesselPacket vp = client->Vessel;

	for (int i = 0; i < 7; i++) {
		bool b;
		if ((b = vp.GetMainControl(1 << i)) != oldVessel.GetMainControl(1 << i)) {
			mc[i] = b;
		}
	}

	for (int i = 0; i < 10; i++) {
		bool b;
		if ((b = vp.GetActionGroup(1 << i)) != oldVessel.GetActionGroup(1 << i)) {
			ag[i] = b;
		}
	}

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(62.f / 256.f, 62.f / 256.f, 61.f / 256.f);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	lastPos = XY{ 16,0 };
	Toggle("Brakes", &mc[4], draw);
	Toggle("Lights", &mc[2], draw);
	Toggle("Gear", &mc[3], draw);
	Toggle("SAS", &mc[0], draw);
	Toggle("RCS", &mc[1], draw);
	for (int i = 0; i < 7; i++) {
		client->Control.SetMainControl(1 << i, mc[i]);
	}

	lastPos.x += 50;

	lastPos.y = 75;
	glLineWidth(12);
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawLine2D(lastPos.x + pos.x, lastPos.y + pos.y, lastPos.x + pos.x, lastPos.y + pos.y + 300);
	draw->BindTextShader();
	draw->SetDrawColor2D(0.75, 0.75, 0.75);
	for (int i = 0; i <= 100; i += 10) {
		draw->DrawString(f, "-" + std::to_string(i), lastPos.x + pos.x + 20, lastPos.y + pos.y + 300 - i * 3 + 5);
	}
	draw->BindDraw2DShader();
	draw->BindTex2D(sliderKnob);
	draw->SetDrawColor2D(1, 1, 1);

	int img_width = 15;
	int img_height = 35;
	int throttleY = Registry["THROTTLE"] * 300 / 1000;
	if (win->MouseDown(SDL_BUTTON_LEFT)) {
		if (drag || (win->MouseX() > lastPos.x + pos.x - img_width && win->MouseY() > lastPos.y + pos.y - img_height + throttleY && win->MouseX() < lastPos.x + pos.x + img_width && win->MouseY() < lastPos.y + pos.y + img_height + throttleY)) {
			drag = true;
			Registry["THROTTLE"] += win->MouseDY() * 1000 / 300;
		}
	}
	else {
		drag = false;
	}
	draw->DrawRectUV2D(lastPos.x + pos.x - img_width, lastPos.y + pos.y - img_height + throttleY, lastPos.x + pos.x + img_width, lastPos.y + pos.y + img_height + throttleY, 0, 0, 0.24609375f, 0.578125f);

	Registry["THROTTLE"] = CLAMP(Registry["THROTTLE"], 0, 1000);

	lastPos.y = 100;
	lastPos.x += 125;
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, "STAGE", lastPos.x + pos.x + 7, lastPos.y + pos.y);
	lastPos.y += 15;
	client->Control.SetMainControl(MC_STAGE, IM::PushButton(draw, win, lastPos + pos) && armStage);
	lastPos.y += 75;
	Toggle("ARM", &armStage, draw);


	lastPos.y = 100;
	lastPos.x += 35;
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, "ABORT", lastPos.x + pos.x + 7, lastPos.y + pos.y);
	lastPos.y += 15;
	client->Control.SetMainControl(MC_ABORT, IM::PushButton(draw, win, lastPos + pos) && armAbort);
	lastPos.y += 75;
	Toggle("ARM", &armAbort, draw);

	lastPos.x = 16;
	lastPos.y = 130;
	Toggle("AG 1", &ag[0], draw);
	Toggle("AG 2", &ag[1], draw);
	Toggle("AG 3", &ag[2], draw);
	Toggle("AG 4", &ag[3], draw);
	Toggle("AG 5", &ag[4], draw);

	lastPos.x = 16;
	lastPos.y += 130;
	Toggle("AG_6", &ag[5], draw);
	Toggle("AG_7", &ag[6], draw);
	Toggle("AG_8", &ag[7], draw);
	Toggle("AG_9", &ag[8], draw);
	Toggle("AG_10", &ag[9], draw);

	for (int i = 0; i < 10; i++) {
		client->Control.SetActionGroup(1 << i, ag[i]);
	}

	lastPos.x = 16;
	lastPos.y += 120;

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

	lastPos.y -= 50;
	SASOption(draw, SAS_HOLD_VECTOR, ptr);

	lastPos.x += 60;
	lastPos.y -= 10;
	float pitch = Registry["SASS_PITCH"], hed = Registry["SASS_HEADING"], roll = Registry["SASS_ROLL"];
	FloatPM(draw, "Pitch", &pitch, 1, 3, true);
	FloatPM(draw, "Heading", &hed, 1, 3, true);
	FloatPM(draw, "Roll", &roll, 1, 3, true);
	Registry["SASS_PITCH"] = pitch;
	Registry["SASS_HEADING"] = hed;
	Registry["SASS_ROLL"] = roll;

	client->Control.SetControlerMode(CONTROLLER_THROTTLE, AXIS_EXT_NZ);

	memcpy(&oldVessel, &vp, sizeof(VesselPacket));
}
