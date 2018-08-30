#pragma once
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define JOY_RANGE(x) short(int(x) * 1000 / 0x8000)
#define JOY_RANGE_THROTTLE(x) short(int(x) * 500 / 0x8000 + 500)

float targetRoll = 0;

int JoyStickCurve(int in, int deadZone) {
	float out = JOY_RANGE(in);
	int sign = out > 0 ? 1 : -1;
	out = abs(out);
	if (out < deadZone) {
		return 0;
	}
	out = (((out - deadZone) * (1000 - 0)) / (1000 - deadZone));
	//NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
	return int(out)*sign;
}

void JoyStickTick() {
	if (client) {
		if (RegInt("ENABLE_FLYBYWIRE", 0)) {
			int deadZone = RegInt("FLYBYWIRE_DEADZONE", 300);
			client->ControlPacket.Roll = JoyStickCurve(win->joystickDir[0], deadZone);
			client->ControlPacket.Pitch = JoyStickCurve(win->joystickDir[1], deadZone);
			client->ControlPacket.Yaw = JoyStickCurve(win->joystickDir[2], deadZone);
			client->ControlPacket.Throttle = 1000 - CLAMP(JOY_RANGE_THROTTLE(win->joystickDir[3]), -1000, 1000);
			if (RegInt("FLYBYWIRE_ROCKETMODE", 0)) {
				float temp = client->ControlPacket.Yaw;
				client->ControlPacket.Yaw = client->ControlPacket.Roll;
				client->ControlPacket.Roll = temp;
				client->ControlPacket.Pitch = -client->ControlPacket.Pitch;
			}
		}
	}
}