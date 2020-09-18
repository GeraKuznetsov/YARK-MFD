#include "Widget.h"

std::string IP = "127.0.0.1";
std::string PORT = "9999";

TextBox tb = TextBox(&IP);
TextBox tb2 = TextBox(&PORT);

Widget::Widget() {

}

std::string Widget::GetTitle() {
	return "Options";
}

#include "../Reg.h"

void RadioOpt(XY pos, std::string option, std::string key) {
	bool val = RegInt(key, 0);
	if (IM::Radio(pos, &val)) {
		Registry[key] = val ? 1 : 0;
	}
	draw->SwitchShader(SHADER_TEXT);
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, option, pos.x + 20, pos.y + 14);
}

#include "../Arduino/EnableArduino.h"

#define TEXT_OFFSET 35
#include "../Arduino/Serial.h"
extern bool ConnectedWithHand();
#if ENABLE_ARDUINO
extern SerialPort* sp;
#endif

void Widget::Draw(XY pos, XY size) {
	draw->SwitchShader(SHADER_TEXT);
	switch (client.GetState()) {
	case TCP_NEW:
		draw->SetTextColor(1, 1, 0);
		draw->DrawString(f, "Client: Not Started", pos.x + 15, pos.y + 40);
		break;
	case TCP_CONNECTING:
		draw->SetTextColor(1, 1, 0);
		draw->DrawString(f, "Client: Connecting... ", pos.x + 15, pos.y + 40);
		break;
	case TCP_CONNECTED:
		draw->SetTextColor(0, 1, 0);
		draw->DrawString(f, "Client: Connected", pos.x + 15, pos.y + 40);
		break;
	case TCP_FAILED:
		draw->SetTextColor(1, 0, 0);
		draw->DrawString(f, "Client: Connection Failed: " + std::string(client.error), pos.x + 15, pos.y + 40);
		break;
	}
	if (client.GetState() != TCP_CONNECTED) {
		IM::TextInput(pos + XY{ 15,40 + 30 }, 100, f, &tb, "Enter IP: ");
		IM::TextInput(pos + XY{ 15,40 + 30 * 2 }, 100, f, &tb2, "Enter PORT: ");
		if (client.GetState() != TCP_CONNECTING) {
			if (IM::Button(pos + XY{ 15,40 + 30 * 3 }, f, "Connect")) {
				client.Connect(IP, PORT);
			}
		}
	}
	RadioOpt(pos + XY{ 15,40 + 30 * 4 }, "Radio Altemeter", "ENABLE_RADIO-ALT");
	draw->SwitchShader(SHADER_TEXT);
	if (win->HasJoyStick()) {
		draw->SetTextColor(0, 1, 0);
		draw->DrawString(f, "JoyStick: Connected", 15, 40 + 30 * 5 + TEXT_OFFSET);
	}
	else {
		draw->SetTextColor(1, 0, 0);
		draw->DrawString(f, "JoyStick: Not Connected", 15, 40 + 30 * 5 + TEXT_OFFSET);
	}
	RadioOpt(pos + XY{ 15  ,40 + 30 * 6 }, "Fly-By-Wire", "ENABLE_FLYBYWIRE");
	RadioOpt(pos + XY{ 15 + 30,40 + 30 * 7 }, "Rocket Mode", "FLYBYWIRE_ROCKETMODE");
	RadioOpt(pos + XY{ 15 + 30,40 + 30 * 8 }, "Joystick Vector Mode", "FORCE_SASS");

	draw->SwitchShader(SHADER_TEXT);
#if ENABLE_ARDUINO
	if (sp && sp->isConnected() && ConnectedWithHand()) {
		draw->SetTextColor(0, 1, 0);
		draw->DrawString(f, "Arduino: Connected", 15, 40 + 30 * 9 + TEXT_OFFSET);
	}
	else {
		draw->SetTextColor(1, 0, 0);
		draw->DrawString(f, "Arduino: Disconnected", 15, 40 + 30 * 9 + TEXT_OFFSET);
	}
#else
	draw->SetTextColor(1, 1, 0);
	draw->DrawString(f, "Arduino: Disabled In build", 15, 40 + 30 * 9 + TEXT_OFFSET);
#endif
}
