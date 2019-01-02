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
	//NewValue = (((OldValue - OldMin) * (NewMax - NewMin)) / (OldMax - OldMin)) + NewMin
	return int(out) * sign;
}

bool smart_ass_was_on = 0;
NavHeading SASS; float SASS_roll;

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

/*
float NAngle(VesselPacket VP) {
	glm::vec4 start = startVec;
	start = glm::rotateY(start, glm::radians(VP.Heading));
	start = glm::rotateX(start, glm::radians(VP.Pitch));
	start = glm::rotateZ(start, glm::radians(VP.Roll));

	/*float tempHeading = glm::radians(VP.Heading);
	float tempPitch = glm::radians(VP.Pitch);
	glm::vec4 AtiVec = glm::vec4(glm::cos(tempPitch) * glm::sin(tempHeading), -glm::sin(tempPitch), -glm::cos(tempPitch) * glm::cos(tempHeading), 1);
	AtiVec = glm::rotateY(AtiVec, glm::radians(VP.Heading));
	AtiVec = glm::rotateX(AtiVec, glm::radians(VP.Pitch));
	AtiVec = glm::rotateZ(AtiVec, glm::radians(VP.Roll));

	glm::vec4 UP = glm::vec4(0, -1, 0, 1);
	UP = glm::rotateY(UP, glm::radians(VP.Heading));
	UP = glm::rotateX(UP, glm::radians(VP.Pitch));
	UP = glm::rotateZ(UP, glm::radians(VP.Roll));

	float angle;
	if (UP.y < 0) {
		angle = atan2(UP.y, UP.x) + glm::radians(90.f);
	}
	else {
		if (UP.x > 0) {
			angle = atan2(UP.y, UP.x) + glm::radians(90.f);
		}
		else {
			angle = -(glm::radians(270.f) - atan2(UP.y, UP.x));
		}
	}

	d->BindDraw2DShader();
	d->BindTex2D(0);
	d->SetDrawColor2D(1, 0, 1);
	d->DrawLine2D(UP.x * 100 + 250, UP.y * 100 + 350, start.x * 100 + 250, start.y * 100 + 350);
	d->SetDrawColor2D(1, 1, 0);
	d->DrawLine2D(UP.x * 100 + 250, UP.y * 100 + 350, 250, 350);

	return angle;
}*/

void JoyStickTick(Client* c, float delta) {
	int deadZone = RegInt("FLYBYWIRE_DEADZONE", 300);
	float roll, pitch, yaw;
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

	if (client) {
		VesselPacket VP = c->vesselPacket;
		if (RegInt("ENABLE_FLYBYWIRE", 0)) {
			client->ControlPacket.Throttle = 1000 - CLAMP(JOY_RANGE_THROTTLE(win->joystickDir[3]), -1000, 1000);
			if (RegInt("FLYBYWIRE_SMART", 0)) {
				client->ControlPacket.SASMode = SAS_HOLD_VECTOR;
				if (!smart_ass_was_on) {
					smart_ass_was_on = true;
					SASS.Pitch = c->vesselPacket.Pitch;
					SASS.Heading = c->vesselPacket.Heading;
					SASS_roll = c->vesselPacket.Roll;
					printf("SASS START");
				}
				float sensitivity = delta * 0.05;
				SASS_roll += -float(roll) * sensitivity * 0.002f;


				//printf(std::to_string(SASS_roll).c_str());
				//printf("\n");
				/*float SASSPitch, SASSYaw;
				glm::vec2 mv = glm::rotate(glm::vec2(pitch / 1000.f, yaw / 1000.f), glm::radians(0.f));
				SASS.Pitch = SASS.Pitch + sensitivity * mv.x;
				SASS.Heading = SASS.Heading + sensitivity * mv.y;


				//printf("%f, %f\n", SASS.Pitch, SASS.Heading);
				//startVec = glm::vec4(glm::cos(glm::radians(VP.Pitch)) * glm::sin(glm::radians(VP.Heading)), -glm::sin(glm::radians(VP.Pitch)), -glm::cos(glm::radians(VP.Pitch)) * glm::cos(glm::radians(VP.Heading)), 1);
				//rollStart = VP.Roll;
				//printf("%f, %f", NavVector.y, 0);*/

				glm::vec2 mv = glm::rotate(glm::vec2(float(pitch) / 1000.f, float(yaw) / 1000.f), glm::radians(0.f));
				printf("craft roll: %f target roll: %f\n", VP.Roll, SASS_roll);
				SASS.Pitch = SASS.Pitch + sensitivity * mv.x;
				SASS.Heading = SASS.Heading + sensitivity * mv.y;

				if (SASS.Pitch > 90.f) {
					SASS.Pitch = 90.f;
				}
				if (SASS.Pitch < -90.f) {
					SASS.Pitch = -90.f;
				}
				if (SASS.Heading > 360) {
					SASS.Heading -= 360;
				}

				client->ControlPacket.targetHeading = SASS.Heading;
				client->ControlPacket.targetPitch = SASS.Pitch;
				client->ControlPacket.targetRoll = SASS_roll;
			//	client->ControlPacket.targetRoll = VP.Roll;
			}
			else
			{
				if (smart_ass_was_on) {
					client->ControlPacket.SASMode = 1;
					smart_ass_was_on = false;
				}
				client->ControlPacket.Yaw = yaw;
				client->ControlPacket.Roll = roll;
				client->ControlPacket.Pitch = pitch;
			}
		}
	}
}