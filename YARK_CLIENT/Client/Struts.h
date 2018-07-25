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

struct vector {
	float x, y, z;
};

struct DataIn {
	int64_t  ID;
	//##### CRAFT ######
	float Pitch;
	float Roll;
	float Heading;

	vector Prograde;

	int16_t ActionGroups; //  status bit order:SAS, RCS, Light, Gear, Brakes, Abort, Custom01 - 10 
	float VVI;
	float G;
	float RAlt;
	float Alt;
	float Vsurf;
	char MaxOverHeat;    //  Max part overheat (% percent)
	float IAS;           //  Indicated Air Speed

					//###### ORBITAL ######
	float VOrbit;
	float AP;
	float PE;
	int TAp;
	int TPe;
	float SemiMajorAxis;
	float SemiMinorAxis;
	float e;
	float inc;
	int period;
	float TrueAnomaly;
	float Lat;
	float Lon;

	//###### FUEL #######
	char CurrentStage;   //  Current stage number
	char TotalStage;     //  TotalNumber of stages
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

	//### MISC ###
	int32_t MissionTime;
	int32_t MNTime;
	float MNDeltaV;
	float TargetDist;    //  Distance to targeted vessel (m)
	float TargetV;       //  Target vessel relative velocity (m/s)
	char SOINumber;      //  SOI Number (decimal format: sun-planet-moon e.g. 130 = kerbin, 131 = mun)
	char NavballSASMode; //  Combined byte for navball target mode and SAS mode
								// First four bits indicate AutoPilot mode:
								// 0 SAS is off  //1 = Regular Stability Assist //2 = Prograde
								// 3 = RetroGrade //4 = Normal //5 = Antinormal //6 = Radial In
								// 7 = Radial Out //8 = Target //9 = Anti-Target //10 = Maneuver node
								// Last 4 bits set navball mode. (0=ignore,1=ORBIT,2=SURFACE,3=TARGET)}
};
#pragma pack(pop)
