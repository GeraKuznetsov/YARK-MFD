#pragma once
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define JOY_RANGE(x) short(int(x) * 1000 / 0x8000)
#define JOY_RANGE_THROTTLE(x) short(int(x) * 500 / 0x8000 + 500)
#define SIGN(x) ((x<0)?((float)-1):((float)1))
float targetRoll = 0;


int JoyStickCurve(int in, int deadZone) {
	float out = JOY_RANGE(in);
	int sign = out > 0 ? 1 : -1;
	out = abs(out);
	if (out < deadZone) {
		return 0;
	}
	out = (((out - deadZone) * (1000 - 0)) / (1000 - deadZone));
	return int(out) * sign;
}

#define GLM_ENABLE_EXPERIMENTAL
#include "gtc\matrix_transform.hpp"
#include "gtx/rotate_vector.hpp"

glm::vec4 toVec(VesselPacket VP, float pitch, float heading) {
	float tempHeading = glm::radians(heading);
	float tempPitch = glm::radians(pitch);
	glm::vec4 out = glm::vec4(glm::cos(tempPitch) * glm::sin(tempHeading), -glm::sin(tempPitch), -glm::cos(tempPitch) * glm::cos(tempHeading), 1);
	out = glm::rotateY(out, glm::radians(VP.Heading));
	out = glm::rotateX(out, glm::radians(VP.Pitch));
	out = glm::rotateZ(out, glm::radians(VP.Roll));
	return out;
}

float s_roll, s_heading, s_pitch;

void JoyStickTick(float delta) {
	int deadZone = RegInt("FLYBYWIRE_DEADZONE", 300);
	float roll, pitch, yaw;
	if (win->HasJoyStick()) {
		if (RegInt("FLYBYWIRE_ROCKETMODE", 0)) {
			yaw = JoyStickCurve(win->joystickDir[0], deadZone);
			roll = JoyStickCurve(win->joystickDir[2], deadZone);
			pitch = -JoyStickCurve(win->joystickDir[1], deadZone);
		}
		else {
			roll = JoyStickCurve(win->joystickDir[0], deadZone);
			pitch = JoyStickCurve(win->joystickDir[1], deadZone);
			yaw = JoyStickCurve(win->joystickDir[2], deadZone);
		}

		VesselPacket VP = client.Vessel;

		if (RegInt("ENABLE_FLYBYWIRE", 0)) {
			client.Control.SetControlerMode(CONTROLLER_THROTTLE, AXIS_EXT_NZ);
			client.Control.SetControlerMode(CONTROLLER_ROT, AXIS_EXT_NZ);
			
			Registry["THROTTLE"] = JOY_RANGE_THROTTLE(win->joystickDir[3]);
			client.Control.Yaw = yaw;
			client.Control.Roll = roll;
			client.Control.Pitch = pitch;
		}
	}
}