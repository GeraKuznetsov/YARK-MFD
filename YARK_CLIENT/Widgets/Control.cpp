#include "Control.h"
#include "Util/IM.h"
#include "../Reg.h"

Control::Control() {
	sliderKnob = loadTexture("Tex/slider.png", false);
	memset((char*)&oldVessel, 0, sizeof(VesselPacket));
}

void Control::Toggle(std::string option, bool* ptr, XY pos) {
	XY renderAt = lastPos + pos;
	IM::ToggleSwitch(renderAt, option, ptr);
	lastPos.x += TOGGLE_SIZE + 20;
}

void Control::SASOption(uint8_t thisMode, XY pos) {
	XY renderAt = lastPos + pos;
	IM::LED(XY{ renderAt.x + 17, renderAt.y }, client.Vessel.SASMode == thisMode);
	renderAt += XY{ 0,38 };
	if (IM::PushButton(renderAt)) {
		client.Control.SASMode = thisMode;
	}
	//draw->BindTex2D(push);
	//draw->DrawRect2D(renderAt.x, renderAt.y, renderAt.x + 64, renderAt.y + 64);
	draw->BindTex2D(TL.SASModeTex(thisMode));
	int inset = 10;
	draw->DrawRect2D(renderAt.x + inset, renderAt.y + inset, renderAt.x + 64 - inset, renderAt.y + 64 - inset);

	lastPos.x += TOGGLE_SIZE + 20;
}
void Control::FloatPM(XY pos, std::string label, float* fl, float grad, int dig, bool sign) {
	draw->SwitchShader(SHADER_TEXT);
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, label, pos.x + lastPos.x - f->GetTextWidth(label) / 2, pos.y + lastPos.y);

	lastPos.y += 18;

	int width = (dig + sign) * 20;

	if (IM::Button(pos + lastPos + XY{ -width / 2 - 26,-7 }, f, "-")) {
		(*fl) -= grad;
	}
	IM::SegInt(pos + lastPos + XY{ -width / 2,-12 }, int(*fl), dig, sign);
	if (IM::Button(pos + lastPos + XY{ width / 2 - 0, -7 }, f, "+")) {
		(*fl) += grad;
	}
	lastPos.y += 40;
}

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

std::string Control::GetTitle() {
	return "Controller";
}

void Control::Draw(XY pos, XY size) {
	VesselPacket vp = client.Vessel;

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

	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(62.f / 256.f, 62.f / 256.f, 61.f / 256.f);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	lastPos = XY{ 16,0 };
	Toggle("Brakes", &mc[4], pos);
	Toggle("Lights", &mc[2], pos);
	Toggle("Gear", &mc[3], pos);
	Toggle("SAS", &mc[0], pos);
	Toggle("RCS", &mc[1], pos);
	for (int i = 0; i < 7; i++) {
		client.Control.SetMainControl(1 << i, mc[i]);
	}

	lastPos.x += 50;

	lastPos.y = 75;
	glLineWidth(12);
	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawLine2D(lastPos.x + pos.x, lastPos.y + pos.y, lastPos.x + pos.x, lastPos.y + pos.y + 300);
	draw->SwitchShader(SHADER_TEXT);
	draw->SetDrawColor2D(0.75, 0.75, 0.75);
	for (int i = 0; i <= 100; i += 10) {
		draw->DrawString(f, "-" + std::to_string(i), lastPos.x + pos.x + 20, lastPos.y + pos.y + 300 - i * 3 + 5);
	}
	draw->SwitchShader(SHADER_2D);
	draw->BindTex2D(sliderKnob);
	draw->SetDrawColor2D(1, 1, 1);

	int img_width = 15;
	int img_height = 35;
	int throttleY = (1000-Registry["THROTTLE"]) * 300 / 1000;
	if (win->MouseDown(SDL_BUTTON_LEFT)) {
		if (drag || (win->MouseX() > lastPos.x + pos.x - img_width && win->MouseY() > lastPos.y + pos.y - img_height + throttleY && win->MouseX() < lastPos.x + pos.x + img_width && win->MouseY() < lastPos.y + pos.y + img_height + throttleY)) {
			drag = true;
			Registry["THROTTLE"] -= win->MouseDY() * 1000 / 300;
		}
	}
	else {
		drag = false;
	}
	draw->DrawRectUV2D(lastPos.x + pos.x - img_width, lastPos.y + pos.y - img_height + throttleY, lastPos.x + pos.x + img_width, lastPos.y + pos.y + img_height + throttleY, 0, 0, 0.24609375f, 0.578125f);

	Registry["THROTTLE"] = CLAMP(Registry["THROTTLE"], 0, 1000);

	lastPos.y = 100;
	lastPos.x += 125;
	draw->SwitchShader(SHADER_TEXT);
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, "STAGE", lastPos.x + pos.x + 7, lastPos.y + pos.y);
	lastPos.y += 15;
	client.Control.SetMainControl(MC_STAGE, IM::PushButton(lastPos + pos) && armStage);
	lastPos.y += 75;
	Toggle("ARM", &armStage, pos);

	lastPos.y = 100;
	lastPos.x += 35;
	draw->SwitchShader(SHADER_TEXT);
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, "ABORT", lastPos.x + pos.x + 7, lastPos.y + pos.y);
	lastPos.y += 15;
	client.Control.SetMainControl(MC_ABORT, IM::PushButton(lastPos + pos) && armAbort);
	lastPos.y += 75;
	Toggle("ARM", &armAbort, pos);

	lastPos.x = 16;
	lastPos.y = 130;
	Toggle("AG 1", &ag[0], pos);
	Toggle("AG 2", &ag[1], pos);
	Toggle("AG 3", &ag[2], pos);
	Toggle("AG 4", &ag[3], pos);
	Toggle("AG 5", &ag[4], pos);

	lastPos.x = 16;
	lastPos.y += 130;
	Toggle("AG_6", &ag[5], pos);
	Toggle("AG_7", &ag[6], pos);
	Toggle("AG_8", &ag[7], pos);
	Toggle("AG_9", &ag[8], pos);
	Toggle("AG_10", &ag[9], pos);

	for (int i = 0; i < 10; i++) {
		client.Control.SetActionGroup(1 << i, ag[i]);
	}

	lastPos.x = 16;
	lastPos.y += 120;

	SASOption( SAS_HOLD, pos);
	SASOption( SAS_PROGRADE, pos);
	SASOption( SAS_NORMAL, pos);
	SASOption( SAS_RADIN, pos);
	SASOption( SAS_TARGET, pos);

	lastPos.x = 16;
	lastPos.y += 120;

	SASOption(SAS_MAN, pos);
	SASOption(SAS_RETROGRADE, pos);
	SASOption(SAS_ANTINORMAL, pos);
	SASOption(SAS_RADOUT, pos);
	SASOption(SAS_ANTITARGET, pos);

	lastPos.y -= 50;
	SASOption(SAS_HOLD_VECTOR, pos);

	lastPos.x += 60;
	lastPos.y -= 10;
	float pitch = Registry["SASS_PITCH"], hed = Registry["SASS_HEADING"], roll = Registry["SASS_ROLL"];
	FloatPM(pos, "Pitch", &pitch, 1, 3, true);
	FloatPM(pos, "Heading", &hed, 1, 3, true);
	FloatPM(pos, "Roll", &roll, 1, 3, true);
	Registry["SASS_PITCH"] = pitch;
	Registry["SASS_HEADING"] = hed;
	Registry["SASS_ROLL"] = roll;

	client.Control.SetControlerMode(CONTROLLER_THROTTLE, AXIS_OVERIDE);

	memcpy(&oldVessel, &vp, sizeof(VesselPacket));
}
