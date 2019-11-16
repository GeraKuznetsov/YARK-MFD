#include "Settings.h"
#include "Util/IM.h"
#include "../Reg.h"

Settings::Settings(WidgetStuff ws) : Widget(ws) {}

void Settings::RadioOpt(std::string option, std::string key, Draw* draw) {
	bool val = RegInt(key, 0);
	XY renderAt = lastPos + this->pos;
	if (IM::Radio(renderAt, win, draw, &val)) {
		Registry[key] = val ? 1 : 0;
	}
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, option, renderAt.x + 20, renderAt.y + 14);
	lastPos.y += 20;
}

#include "Console.h"
extern Console* console;

void Settings::Tick(Draw* draw) {
	WindowUpdate(draw);
	lastPos = XY{ 0,0 };
	RadioOpt("Save State On Exit", "ENABLE_SAVE_ON_EXIT", draw);
	if (IM::Button(XY{ pos.x + 2,pos.y + 20 }, win, draw, f, "Save State")) {
		console->command("savestate");
	}
	lastPos.y += 20;
	RadioOpt("Radio Altimeter", "ENABLE_RADIO-ALT", draw);
	RadioOpt("Window Frames", "ENABLE_WINDOW_FRAMES", draw);

	draw->BindTextShader();
	if (win->HasJoyStick()) {
		draw->SetTextColor(0, 1, 0);
		draw->DrawString(f, "Joy-Stick Connected", pos.x + lastPos.x + 5, pos.y + lastPos.y + 13);
	}
	else {
		draw->SetTextColor(1, 0, 0);
		draw->DrawString(f, "No Joy-Stick Connected", pos.x + lastPos.x + 5, pos.y + lastPos.y + 13);
	} lastPos.y += 20;


	RadioOpt("Fly-By-Wire", "ENABLE_FLYBYWIRE", draw);
	RadioOpt("Rocket Mode", "FLYBYWIRE_ROCKETMODE", draw);
}
