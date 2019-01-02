#pragma once
#pragma pack(push, 1)

#define MC_STAGE 1 << 0
#define MC_ABORT 1 << 1
#define MC_PRECISION 1 << 2
#define MC_BRAKES 1 << 3
#define MC_GEAR 1 << 4
#define MC_LIGHTS 1 << 5
#define MC_RCS 1 << 6
#define MC_SAS 1 << 7

#define AG_1 1 << 1
#define AG_2 1 << 2
#define AG_3 1 << 3
#define AG_4 1 << 4
#define AG_5 1 << 5
#define AG_6 1 << 6
#define AG_7 1 << 7
#define AG_8 1 << 8
#define AG_9 1 << 9
#define AG_10 1 << 10

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
#define TIMEWARP_x2f 1
#define TIMEWARP_x3f 2
#define TIMEWARP_x4f 3
#define TIMEWARP_x5 4
#define TIMEWARP_x10 5
#define TIMEWARP_x50 6
#define TIMEWARP_x100 7
#define TIMEWARP_x1000 8
#define TIMEWARP_x10000 9
#define TIMEWARP_x100000 10

struct ControlPacket
{
	int8_t HEADER_0;

	int64_t ID;
	int8_t MainControls;                  //SAS RCS Lights Gear Brakes Precision Abort Stage 
	int8_t Mode;                          //0 = stage, 1 = docking, 2 = map
	int16_t ControlGroup;                //control groups 1-10 in 2 chars
	int16_t Pitch;                        //-1000 -> 1000
	int16_t Roll;                         //-1000 -> 1000
	int16_t Yaw;                          //-1000 -> 1000
	int16_t TX;                           //-1000 -> 1000
	int16_t TY;                           //-1000 -> 1000
	int16_t TZ;                           //-1000 -> 1000
	int16_t WheelSteer;                   //-1000 -> 1000
	int16_t Throttle;                     // 0 -> 1000
	int16_t WheelThrottle;                // 0 -> 1000
	int8_t SASMode; //hold, prograde, retro, etc...
	int8_t SpeedMode; //Surface, orbit target
    float targetHeading, targetPitch, targetRoll;
	int8_t timeWarpRateIndex;
};

struct Header {
	int8_t HEADER_0;
	int8_t packetType;
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

	int16_t ActionGroups; //  status bit order:SAS, RCS, Light, Gear, Brakes, Abort, Custom01 - 10 
	float VVI;
	float G;
	float RAlt;
	float Alt;
	float Vsurf;
	int8_t MaxOverHeat;    //  Max part overheat (% percent)
	float IAS;           //  Indicated Air Speed


							  //###### ORBITAL ######
	float VOrbit;
	float AP;
	float PE;
	int32_t TAp;
	int32_t TPe;
	float SemiMajorAxis;
	float SemiMinorAxis;
	float e;
	float inc;
	int32_t period;
	float TrueAnomaly;
	float Lat;
	float Lon;

	//###### FUEL #######
	int8_t CurrentStage;   //  Current stage number
	int8_t TotalStage;     //  TotalNumber of stages
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
	int8_t HasTarget;
	float TargetDist;    //  Distance to targeted vessel (m)
	float TargetV;       //  Target vessel relative velocity (m/s)
	int8_t SOINumber;      //  SOI Number (decimal format: sun-planet-moon e.g. 130 = kerbin, 131 = mun)

	int8_t SASMode; //hold, prograde, retro, etc...
	int8_t SpeedMode; //Surface, orbit target
	int8_t timeWarpRateIndex;
};
#pragma pack(pop)