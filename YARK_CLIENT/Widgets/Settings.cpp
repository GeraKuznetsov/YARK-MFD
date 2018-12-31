#include "Settings.h"
#include "Util\Button.h"
#include "../Reg.h"
Settings::Settings(WidgetStuff ws) : Widget(ws) {

}

void Settings::RadioOpt(std::string option, std::string key, XY pos, Draw* draw) {
	bool val = RegInt(key, 0);
	pos += this->pos;
	if (IM::Radio(pos, win, draw, &val)) {
		Registry[key] = val ? 1 : 0;
	}
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f,option, pos.x + 20, pos.y + 14);
}

#include "Console.h"
extern Console* console;

void Settings::Tick(Draw* draw) {
	WindowUpdate(draw);
	RadioOpt("Save State On Exit", "ENABLE_SAVE_ON_EXIT", XY{ 0,0 }, draw);
	if (IM::Button(XY{ pos.x+2,pos.y+20 }, win, draw, f, "Save State")) {
		console->command("savestate");
	}
	RadioOpt("Radio Altimeter", "ENABLE_RADIO-ALT", XY{ 0,40 }, draw);
	RadioOpt("Window Frames", "ENABLE_WINDOW_FRAMES", XY{ 0,60 }, draw);
	if (win->HasJoyStick()) {
		RadioOpt("Fly-By-Wire", "ENABLE_FLYBYWIRE", XY{ 0,40 }, draw);
		RadioOpt("Rocket Mode", "FLYBYWIRE_ROCKETMODE", XY{ 0,60 }, draw);
		RadioOpt("Smart Attitude control", "FLYBYWIRE_SMART", XY{ 0,80 }, draw);
	}
}
int Settings::Input() {
	return Input();
}