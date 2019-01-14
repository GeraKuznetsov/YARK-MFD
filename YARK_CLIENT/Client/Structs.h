#pragma once
#pragma pack(push, 1)

//defines for MainControls
#define MC_SAS (1 << 0)
#define MC_RCS (1 << 1)
#define MC_LIGHTS (1 << 2)
#define MC_GEAR (1 << 3)
#define MC_BRAKES (1 << 4)
#define MC_ABORT (1 << 5)
#define MC_STAGE (1 << 6)

#define AG_1 (1 << 0)
#define AG_2 (1 << 1)
#define AG_3 (1 << 2)
#define AG_4 (1 << 3)
#define AG_5 (1 << 4)
#define AG_6 (1 << 5)
#define AG_7 (1 << 6)
#define AG_8 (1 << 7)
#define AG_9 (1 << 8)
#define AG_10 (1 << 9)

//These are SAS modes 
#define SAS_HOLD 1
#define SAS_PROGRADE 2
#define SAS_RETROGRADE 3
#define SAS_NORMAL 4
#define SAS_ANTINORMAL 5
#define SAS_RADIN 6
#define SAS_RADOUT 7
#define SAS_TARGET 8
#define SAS_ANTITARGET 9
#define SAS_MAN 10
#define SAS_HOLD_VECTOR 11

#define TIMEWARP_x1 0
#define TIMEWARP_x2p 1
#define TIMEWARP_x3p 2
#define TIMEWARP_x4p 3
#define TIMEWARP_x5 4
#define TIMEWARP_x10 5
#define TIMEWARP_x50 6
#define TIMEWARP_x100 7
#define TIMEWARP_x1000 8
#define TIMEWARP_x10000 9
#define TIMEWARP_x100000 10

const uint8_t Header_Array[8] = { (uint8_t)0xFF, (uint8_t)0xC4, (uint8_t)'Y', (uint8_t)'A', (uint8_t)'R', (uint8_t)'K', (uint8_t)0x00, (uint8_t)0xFF };

struct ControlPacket
{
	uint8_t header[8];
	uint64_t ID;
	uint8_t MainControls;                   //SAS RCS Lights Gear Brakes Abort Stage
	uint16_t ActionGroups;                //action groups 1-10 in 2 bytes
	int16_t Pitch;                        //-1000 -> 1000
	int16_t Roll;                         //-1000 -> 1000
	int16_t Yaw;                          //-1000 -> 1000
	int16_t TX;                           //-1000 -> 1000
	int16_t TY;                           //-1000 -> 1000
	int16_t TZ;                           //-1000 -> 1000
	int16_t WheelSteer;                   //-1000 -> 1000
	int16_t Throttle;                     // 0 -> 1000
	int16_t WheelThrottle;                // 0 -> 1000
	uint8_t SASMode; //hold, prograde, retro, etc...
	uint8_t SpeedMode; //Surface, orbit target
	float targetHeading, targetPitch, targetRoll;
	uint8_t timeWarpRateIndex;
};

struct StatusPacket {
	int64_t ID;
	int8_t inFlight;
	char vessalName[16];
};

struct NavHeading {
	float Pitch, Heading;
	NavHeading() {
		Pitch = Heading = 0;
	}
	NavHeading(float pitch, float heading) {
		this->Pitch = pitch;
		this->Heading = heading;
	}
};

struct VesselPacket {
	uint64_t ID;

	float deltaTime;

	//##### CRAFT ######
	float Pitch; //pitch and heading close together so c++ can use this as a NavHeading ptr
	float Heading;
	float Roll;

	//#### NAVBALL VECTOR #######
	NavHeading Prograde;
	NavHeading Target;
	NavHeading Maneuver;

	uint8_t MainControls;                   //SAS RCS Lights Gear Brakes Abort Stage
	uint16_t ActionGroups;                   //action groups 1-10 in 2 bytes
	float VVI;
	float G;
	float RAlt;
	float Alt;
	float Vsurf;
	uint8_t MaxOverHeat;    //  Max part overheat (% percent)
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
	uint8_t CurrentStage;   //  Current stage number
	uint8_t TotalStage;     //  TotalNumber of stages
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
	uint32_t MissionTime;
	uint32_t MNTime;
	float MNDeltaV;
	uint8_t HasTarget;
	float TargetDist;    //  Distance to targeted vessel (m)
	float TargetV;       //  Target vessel relative velocity (m/s)
	uint8_t SOINumber;      //  SOI Number (decimal format: sun-planet-moon e.g. 130 = kerbin, 131 = mun)

	uint8_t SASMode; //hold, prograde, retro, etc...
	uint8_t SpeedMode; //Surface, orbit target

	uint8_t timeWarpRateIndex;
};
#pragma pack(pop)