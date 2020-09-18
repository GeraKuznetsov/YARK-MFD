#pragma once

#include "EnableArduino.h"
#if ENABLE_ARDUINO 
#include "../Client/Client.h"
extern Client client;
#include "../Reg.h"
#include "Serial.h"
#include <string>
#include <thread>
SerialPort* sp;

typedef unsigned char byte;

#define HEADER_LENGTH 4
byte header[HEADER_LENGTH] = { 'Y', 'A', 'R', 'K' };

#pragma pack(push, 1)
struct {
	byte header[HEADER_LENGTH];

	/*Delete the following add add what you want to send to a connected arduino*/
	byte analog[8];
	byte max[12][8];
	/*end section*/
}DataOut;

struct {
	byte header[HEADER_LENGTH];

	/*Delete the following add add what you want to recieve from a connected arduino*/
	byte switches[5];
	uint16_t throttle;
	/*end section*/

}DataIn;

#pragma pack(pop)

#pragma region helper methods
#include <glm.hpp>
byte getVertSpeedValue(float VS) {
	float e = 0.001;
	float sign;
	if (VS > e) {
		sign = 1.f;
	}
	else if (VS < -e) {
		sign = -1.f;
		VS = -VS;
	}
	else {
		return 0;
	}
	if (VS > 19.f) {
		VS = 19;
	}
	float angle = log(VS + 1) / log(21);
	return (byte)(128.f - (sign * angle * 128.f));
}


byte BCD[10] = {
	0b01111110,
	0b00110000,
	0b01101101,
	0b01111001,
	0b00110011,
	0b01011011,
	0b01011111,
	0b01110000,
	0b01111111,
	0b01110011
};

const byte neg = 0b00000001;

byte getBCD(byte i, bool dec = false) {
	//return i;
	return BCD[i] | (dec << 7);
}

void format_float_4_chars(byte* data, float f, bool sign, bool dec) {
	if (dec)
		f *= 10;
	int INT = round(abs(f));
	int chars = sign ? 3 : 4;
	for (int i = 0; i < chars; i++) {
		int iPart = INT % 10;
		*(data + i) = getBCD(iPart, (i == 1) && dec);
		INT /= 10;
	}
	if (sign && f < 0) {
		*(data + 3) = neg;
	}
}

void format_float_8_chars(byte* data, float f, bool dec) {
	if (f > 9999999.9 || isnan(f)) {
		for (int i = 0; i < 8; i++) {
			*(data + i) = neg;
		}
	}
	else {
		if (dec)
			f *= 10;
		int INT = round(f);
		for (int i = 0; i < 8; i++) {
			int iPart = INT % 10;
			*(data + i) = getBCD(iPart, i == 1 && dec);
			INT /= 10;
		}
	}
}

float rad[] = { 261600000.f ,250000.f  ,700000.f  ,	13000.f  ,	600000.f,	200000.f  ,	60000.f,	320000.f ,	130000.f  ,	138000.f ,	6000000.f,	500000.f,	300000.f ,	600000.f ,	65000.f   ,	44000.f   ,	210000.f };

int I = 0, test = 0;

std::string byte_to_binary(byte x) {
	std::string out;
	for (byte mask = 0b10000000; mask; mask >>= 1) {
		out += (x & mask) ? '1' : '0';
	}
	return out;
}

#define BIT(x, n) ((x) & (1 << n))
#define TEST(n, m) (!((test + n) % m))
//Hold       Prograde   Normal     RadialIn  Target
//HoldVector Retrograde AntiNormal RadialOut AntiTarget Man
//SRF TRG ORB

float throttle_in = 0;

#define CB(a,b,c,d,e,f,g,h) (((a) << 0) | ((b) << 1) | ((c) << 2) | ((d) << 3) | ((e) << 4) | ((f) << 5) | ((g) << 6) | ((h) << 7))
#define GMC(x) client.Vessel.GetMainControl(x)
#define GAG(x) client.Vessel.GetActionGroup(x)
#define CV client.Vessel
#define CC client.Control

byte fuelValue(float* per, float a0, float b0, float a1, float b1, bool choose) {
	if (choose) {
		*per = a0 / b0;
	}
	else {
		*per = a1 / b1;
	}
	return byte(*per * 255.f);
}

byte fuelValue(float* per, float a0, float b0) {
	return fuelValue(per, a0, b0, a0, b0, true);
}

bool blink(int state, int i) { //0 == off, 1 == blink 2 == on
	return ((state == 1) && ((i / 4) % 2)) || (state == 2);
}

byte fuel_state(float per) {
	if (per > 0.30f) {
		return 0;
	}
	else if (per > 0.15f) {
		return 1;
	}return 2;
}

byte heat_state(float per) {
	if (per > 0.60f) {
		return 2;
	}
	else if (per > 0.40f) {
		return 1;
	}return 0;
}
#pragma endregion

//this method fills dataout with data to be sent to arduino
void ArduinoOutput() {
	for (int i = 0; i < 8; i++) {
		for (int n = 0; n < 12; n++) {
			DataOut.max[n][i] = 0;
		}
	}
	//OXY HEAT EC
	float per;
	DataOut.analog[1] = fuelValue(&per, CV.G, 10.f);

	DataOut.analog[2] = fuelValue(&per, CV.MonoProp, CV.MonoPropTot); //Mono
	DataOut.max[9][3] |= blink(fuel_state(per), I) << 2;

	DataOut.analog[3] = fuelValue(&per, CV.SolidFuel, CV.SolidFuelTot); //SF
	DataOut.max[9][3] |= blink(fuel_state(per), I) << 1;

	DataOut.analog[4] = fuelValue(&per, CV.LiquidFuel, CV.LiquidFuelTot, CV.LiquidFuelS, CV.LiquidFuelTotS, true); //LF
	DataOut.max[9][3] |= blink(fuel_state(per), I) << 0;

	DataOut.analog[5] = fuelValue(&per, CV.Oxidizer, CV.OxidizerTot, CV.OxidizerS, CV.OxidizerTotS, true); //oxy
	DataOut.max[9][2] |= blink(fuel_state(per), I) << 0;

	DataOut.analog[6] = fuelValue(&per, CV.MaxOverHeat, 100.f); //heat
	DataOut.max[9][2] |= blink(heat_state(per), I) << 1;

	DataOut.analog[7] = fuelValue(&per, CV.ECharge, CV.EChargeTot); //EC
	DataOut.max[9][2] |= blink(fuel_state(per), I) << 2;


	DataOut.max[9][3] |= (blink(2, I) << 3);
	DataOut.max[9][2] |= (blink(2, I) << 3);

	I++;
	format_float_8_chars(DataOut.max[0], CV.Alt, true);
	float speed = -1;
	switch (CV.SpeedMode) {
	case 1: speed = CV.VOrbit; break;
	case 2: speed = CV.Vsurf; break;
	case 3: speed = CV.TargetV; break;
	default: speed = CV.VOrbit;
	}
	format_float_8_chars(DataOut.max[1], speed, true);

	format_float_4_chars(DataOut.max[2], CV.Pitch, true, true);
	format_float_4_chars(DataOut.max[2] + 4, throttle_in / 10, false, false);
	format_float_4_chars(DataOut.max[3] + 0, CV.CurrentStage * 100 + CV.TotalStage, false, false);
	format_float_4_chars(DataOut.max[3] + 4, CV.TargetDist, false, false);
	format_float_4_chars(DataOut.max[8], CV.Heading, false, true);
	format_float_4_chars(DataOut.max[8] + 4, CV.Roll, true, false);
	bool dispPE = CV.CurrentOrbit.PE > 0;
	format_float_8_chars(DataOut.max[4], dispPE ? CV.CurrentOrbit.PE : NAN, false);
	format_float_8_chars(DataOut.max[5], CV.CurrentOrbit.AP, false);
	format_float_8_chars(DataOut.max[6], dispPE ? CV.CurrentOrbit.T2Pe : NAN, false);
	int t2ap = -1;
	if (CV.CurrentOrbit.anomoly > 0) {
		t2ap = CV.CurrentOrbit.T2Pe - CV.CurrentOrbit.period / 2;
	}
	else {
		t2ap = CV.CurrentOrbit.T2Pe + CV.CurrentOrbit.period / 2;
	}
	format_float_8_chars(DataOut.max[7], t2ap, false);

	DataOut.max[9][7] = CB(GMC(MC_GEAR), GMC(MC_LIGHTS), GMC(MC_BRAKES), GMC(MC_RCS), CV.SASMode == SAS_ANTINORMAL, CV.SASMode == SAS_NORMAL, CV.SASMode == SAS_PROGRADE, CV.SASMode == SAS_HOLD);
	if (!(I % 5)) {
		test++;
	}
	DataOut.max[9][5] = CB(GAG(AG_5), GAG(AG_4), GAG(AG_3), GAG(AG_2), GAG(AG_1), GMC(MC_SAS), CV.SASMode == SAS_TARGET, CV.SASMode == SAS_RADOUT);
	DataOut.max[9][6] = CB(GAG(AG_10), GAG(AG_9), GAG(AG_8), GAG(AG_7), GAG(AG_6), CV.SASMode == SAS_MAN, CV.SASMode == SAS_ANTITARGET, CV.SASMode == SAS_RADIN);
	DataOut.max[9][4] = CB(0, 0, 0, CV.SpeedMode == 3, CV.SpeedMode == 1, CV.SpeedMode == 2, CV.SASMode == SAS_RETROGRADE, CV.SASMode == SAS_HOLD_VECTOR);

	bool dispMan = client.OrbitPlan.Mans.size() != 0;
	format_float_8_chars(DataOut.max[10], dispMan ? CV.MNTime : NAN, false);
	format_float_8_chars(DataOut.max[11], dispMan ? CV.MNDeltaV : NAN, false);

	for (int i = 0; i < 8; i++) {
		DataOut.max[9][i] = blink(1, I) ? 0xFF : 0;
	}
}

//this method takes code from DataIn struct and uses it to update the controls
void ArduinoInput() {
	CC.SetControlerMode(CONTROLLER_THROTTLE, AXIS_OVERIDE);

	byte row = DataIn.switches[0]; //ROW 0
	(!BIT(row, 0)); //OPT xxx
	CC.SetMainControl(MC_GEAR, BIT(row, 1));    //GEAR
	CC.SetMainControl(MC_LIGHTS, BIT(row, 2));  //LIGHTS
	CC.SetMainControl(MC_BRAKES, BIT(row, 3));  //BREAKS
	CC.SetMainControl(MC_RCS, BIT(row, 4));     //RCS
	!BIT(row, 5); //OPT xxx
	!BIT(row, 6); //OPT xxx
	!BIT(row, 7); //OPT xxx

	row = DataIn.switches[1]; //ROW 1
	CC.SetActionGroup(AG_10, BIT(row, 0));      //AG 10
	CC.SetActionGroup(AG_9, BIT(row, 1));       //AG 9
	CC.SetActionGroup(AG_8, BIT(row, 2));       //AG 8
	CC.SetActionGroup(AG_7, BIT(row, 3));       //AG 7
	CC.SetActionGroup(AG_6, BIT(row, 4));       //AG 6
	if (!BIT(row, 5)) CC.SASMode = SAS_MAN;
	if (!BIT(row, 6)) CC.SASMode = SAS_ANTITARGET;
	if (!BIT(row, 7)) CC.SASMode = SAS_RADIN;

	row = DataIn.switches[2]; //ROW 2					   
	CC.SetActionGroup(AG_5, BIT(row, 0));       //AG 5
	CC.SetActionGroup(AG_4, BIT(row, 1));       //AG 4
	CC.SetActionGroup(AG_3, BIT(row, 2));       //AG 3
	CC.SetActionGroup(AG_2, BIT(row, 3));       //AG 2
	CC.SetActionGroup(AG_1, BIT(row, 4));       //AG 1
	CC.SetMainControl(MC_SAS, BIT(row, 5));     //SAS
	if (!BIT(row, 6))  CC.SASMode = SAS_TARGET;
	if (!BIT(row, 7))  CC.SASMode = SAS_RADOUT;

	row = DataIn.switches[3]; //ROW 3
	if (!BIT(row, 0)) CC.SpeedMode = 3;
	if (!BIT(row, 1)) CC.SASMode = SAS_ANTINORMAL;
	if (!BIT(row, 2)) CC.SASMode = SAS_NORMAL;
	if (!BIT(row, 3)) CC.SpeedMode = 1;
	if (!BIT(row, 4)) CC.SASMode = SAS_RETROGRADE;
	if (!BIT(row, 5)) CC.SASMode = SAS_PROGRADE;
	if (!BIT(row, 6)) CC.SASMode = SAS_HOLD_VECTOR;
	if (!BIT(row, 7)) CC.SASMode = SAS_HOLD;

	row = DataIn.switches[4]; //ROW 4
	CC.SetMainControl(MC_STAGE, !BIT(row, 0));
	CC.SetMainControl(MC_ABORT, !BIT(row, 1));
	if (!BIT(row, 2)) CC.SpeedMode = 2;
	Registry["THROTTLE"] = throttle_in = int(float(DataIn.throttle) * 1000.f / 1024.f);
}


bool handshake() {
	sp->writeBytes((byte*)&DataOut, HEADER_LENGTH);
	if (sp->readBytes(DataIn.header, HEADER_LENGTH)) {
		return !memcmp(DataIn.header, header, HEADER_LENGTH);
	}
	return false;
}

bool arduinoRunning = false;

void ArduinoConnect() {
	memcpy(DataOut.header, header, HEADER_LENGTH);
	printf("Searching for hardware...\n");
	char port[32];
	for (int i = 0; i < 10; i++) {
		printf("Trying port %d\n", i);
		sprintf_s(port, "\\\\.\\COM%d", i);
		sp = new SerialPort(port);
		if (sp->isConnected()) {
			if (handshake()) {
				printf("	handshake success\n");
				goto run;
			}
			else {
				printf("	handshake failed\n");
			}
		}
		delete sp; sp = 0;
	}
run:;
	while (sp) {
		arduinoRunning = true;
		ArduinoOutput();
		sp->writeBytes((byte*)&DataOut, sizeof(DataOut));
		if (sp->readBytes((byte*)&DataIn, sizeof(DataIn))) {
			if (!memcmp(DataIn.header, header, HEADER_LENGTH)) {
				ArduinoInput();
			}
		}
		else {
			delete sp; sp = 0;
		}
	}
	arduinoRunning = false;
}

void RunArduino() {
	arduinoRunning = false;
	std::thread t(ArduinoConnect);
	t.detach();
}
bool ConnectedWithHand() {
	return arduinoRunning;
}
#else
bool ConnectedWithHand() {
	return false;
}
#endif 
