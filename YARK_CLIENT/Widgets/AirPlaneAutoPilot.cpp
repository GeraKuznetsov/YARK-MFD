#include "AirPlaneAutoPilot.h"
#include "Util/IM.h"
#include "../Reg.h"

#undef min
#undef max

std::string spdtxt = "200";
std::string alttxt = "2000";
std::string maxpitchtxt = "10";
std::string maxaoatxt = "5";
TextBox inSpeed = TextBox(&spdtxt, TEXT_ALLOW_NUM);
TextBox inAlt = TextBox(&alttxt, TEXT_ALLOW_NUM);
TextBox inMP = TextBox(&maxpitchtxt, TEXT_ALLOW_NUM);
TextBox inMAOA = TextBox(&maxaoatxt, TEXT_ALLOW_NUM);

AirPlaneAutoPilot::AirPlaneAutoPilot() {
	RegInt("AP_MAX_AOA", 10);
	RegInt("AP_SPEED", 1000);
	RegInt("AP_ALT", 2000);
}

#define SIGN(x) ((x<0)?((float)-1):((float)1))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

std::string AirPlaneAutoPilot::GetTitle() {
	return "AirPlane auto Pilot";
}

void AirPlaneAutoPilot::Draw(XY pos, XY size) {
	draw->SwitchShader(SHADER_2D);
	draw->SetDrawColor2D(62.f / 256.f, 62.f / 256.f, 61.f / 256.f);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	int y = 0;

	IM::Radio(pos + XY{ 40,40 + 30 * y++ }, &maintainAlt, f, "Maintain altitude");
	IM::Radio(pos + XY{ 40,40 + 30 * y++ }, &maintainSpeed, f, "Maintain speed");
	draw->SwitchShader(SHADER_TEXT);
	draw->SetTextColor(1, 1, 1);
	if (IM::Button(pos + XY{ 40,40 + 30 * y++ }, f, "Target Altitude: " + std::to_string(targetAlt))) {
		open = (open == 1) ? 0 : 1;
	}
	if (open == 1) {
		IM::TextInput(pos + XY{ 40,40 + 30 * y++ }, 100, f, &inAlt, "Input Target Altitude: ");
		if (IM::Button(pos + XY{ 40,40 + 30 * y++ }, f, "Set")) {
			targetAlt = std::stoi(alttxt);
		}
	}
	if (IM::Button(pos + XY{ 40,40 + 30 * y++ }, f, "Target Speed: " + std::to_string(targetSpeed))) {
		open = (open == 2) ? 0 : 2;
	}
	if (open == 2) {
		IM::TextInput(pos + XY{ 40,40 + 30 * y++ }, 100, f, &inSpeed, "Input Target Speed: ");
		if (IM::Button(pos + XY{ 40,40 + 30 * y++ }, f, "Set")) {
			targetSpeed = std::stoi(spdtxt);
		}
	}
	if (IM::Button(pos + XY{ 40,40 + 30 * y++ }, f, "Max pitch angle: " + std::to_string(maxPitch))) {
		open = (open == 3) ? 0 : 3;
	}
	if (open == 3) {
		IM::TextInput(pos + XY{ 40,40 + 30 * y++ }, 100, f, &inMP, "Input Max Pitch: ");
		if (IM::Button(pos + XY{ 40,40 + 30 * y++ }, f, "Set")) {
			maxPitch = std::stoi(maxpitchtxt);
		}
	}
	if (IM::Button(pos + XY{ 40,40 + 30 * y++ }, f, "Max AoA: " + std::to_string(maxAoA))) {
		open = (open == 4) ? 0 : 4;
	}
	if (open == 4) {
		IM::TextInput(pos + XY{ 40,40 + 30 * y++ }, 100, f, &inMAOA, "Input Max AoA: ");
		if (IM::Button(pos + XY{ 40,40 + 30 * y++ }, f, "Set")) {
			maxAoA = std::stoi(maxaoatxt);
		}
	}

	if (maintainAlt) {
		client.Control.SetMainControl(MC_SAS, true);
		client.Control.SASMode = SAS_HOLD_VECTOR;

		float deltaAlt = targetAlt - client.Vessel.Alt;
		Registry["SASS_HEADING"] = client.Vessel.Heading;
		Registry["SASS_ROLL"] = client.Vessel.Roll;

		float pitchTarg;
		float result;
		if (abs(deltaAlt) < 200) {
			result = glm::max(sqrt(abs(deltaAlt / 200)) * maxPitch, 0.1f) * SIGN(deltaAlt);
		}
		else {
			result = maxPitch * SIGN(deltaAlt);
		}
		if (result > client.Vessel.Prograde.Pitch + maxAoA) {
			result = client.Vessel.Prograde.Pitch + maxAoA;
		}
		if (result < client.Vessel.Prograde.Pitch - maxAoA) {
			result = client.Vessel.Prograde.Pitch - maxAoA;
		}
		Registry["SASS_PITCH"] = result;
	}
	if (maintainSpeed) {
		client.Control.SetControlerMode(CONTROLLER_THROTTLE, AXIS_OVERIDE);
		if (client.Vessel.ID > lastID) { //If we got a new packet
			printf("a\n");
			double thisTime = client.Vessel.MissionTime;
			double dTime = thisTime - lastTime; //Calulate change in time between packets
			if (dTime > 0.001) {
				lastTime = thisTime;

				double thisSpead = client.Vessel.Vsurf;
				double dSpeed = thisSpead - lastSpead; //Calculate change in speed between packets
				lastSpead = thisSpead;
				printf("%f\n", dSpeed);

				float p = targetSpeed - client.Vessel.Vsurf;
				float d = -(dSpeed / dTime) / 10;
				float thr = glm::min(glm::max(p + d, 0.f), 1.f);
				Registry["THROTTLE"] = 1000.f * thr;
			}
			else if (thisTime < lastTime) {
				lastTime = 0;
			}
		}
	}
}
