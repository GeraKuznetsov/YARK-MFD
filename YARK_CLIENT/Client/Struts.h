#pragma once


#pragma pack(push, 1)
struct ControlPacket
{
	 char HEADER_0;
	 char HEADER_1;
	 int64_t  ID;
	 char MainControls;                  //SAS RCS Lights Gear Brakes Precision Abort Stage 
	 char Mode;                          //0 = stage, 1 = docking, 2 = map
	 int16_t ControlGroup;                //control groups 1-10 in 2 bytes
	 int16_t Pitch;                        //-1000 -> 1000
	 int16_t Roll;                         //-1000 -> 1000
	 int16_t Yaw;                          //-1000 -> 1000
	 int16_t TX;                           //-1000 -> 1000
	 int16_t TY;                           //-1000 -> 1000
	 int16_t TZ;                           //-1000 -> 1000
	 int16_t WheelSteer;                   //-1000 -> 1000
	 int16_t Throttle;                     // 0 -> 1000
	 int16_t WheelThrottle;                // 0 -> 1000
	 char NavballSASMode;                //AutoPilot mode (See above for AutoPilot modes)(Ignored if the equal to zero or out of bounds (>10)) //Navball mode
};

struct Header {
	char HEADER_0;
	char HEADER_1;
	char packetType;
};

struct Status {
	char status;
	char vessalName[16];
};

struct DataIn {
	int64_t  ID;
	float Roll;
	float Pitch;
	float Heading;
	float t1, t2, t3, t4;
	float Lat;
	float Lon;
	float LiquidFuelTot;
	float LiquidFuel;
	float OxidizerTot;
	float Oxidizer;
	float EChargeTot;
	float ECharge;
	float MonoPropTot;
	float MonoProp;
	float IntakeAirTot;
	float IntakeAir;
	float SolidFuelTot;
	float SolidFuel;
	float XenonGasTot;
	float XenonGas;
	float LiquidFuelTotS;
	float LiquidFuelS;
	float OxidizerTotS;
	float OxidizerS;
};
#pragma pack(pop)
