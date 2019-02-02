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

//Action group flags
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

//SAS mode definitions
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

//Timewarp mode definitions
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

//For enableing / disableing axis input
#define CONTROLLER_ROT 0
#define CONTROLLER_TRANS 1
#define CONTROLLER_THROTTLE 2
#define CONTROLLER_WHEEL 3

#define AXIS_IGNORE 0 //Always uses interal KSP value, ignoring client value
#define AXIS_OVERIDE 1 //Client always used overrides KSP value 
#define AXIS_INT_NZ 2 //Client value is used if the internal KSP value is zero, otherwise interal KSP value is used (KSP interal value overrides client value)
#define AXIS_EXT_NZ 3 //Interal KSP value is used if the client value is zero, otherwise client value is sent (Client value overrides KSP internal value)

const uint8_t Header_Array[8] = { (uint8_t)0xFF, (uint8_t)0xC4, (uint8_t)'Y', (uint8_t)'A', (uint8_t)'R', (uint8_t)'K', (uint8_t)0x00, (uint8_t)0xFF };

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

struct Header {
	uint8_t header[8];
	uint16_t checksum;
	uint8_t type;
};

struct ControlPacket
{
	Header header;
	uint32_t ID;
	uint8_t MainControls;                   //SAS RCS Lights Gear Brakes Abort Stage
	uint16_t ActionGroups;                //action groups 1-10 in 2 bytes
	   // Throttle and axis controls have the following settings: 
	   // 0: The internal value (supplied by KSP) is always used.
	   // 1: The external value (read from serial packet) is always used.
	   // 2: If the internal value is not zero use it, otherwise use the external value.
	   // 3: If the external value is not zero use it, otherwise use the internal value.  
	uint8_t ControlerMode;                //DDCCBBAA (2 bits each)
	float SASTol;
	int16_t Pitch;                        //-1000 -> 1000 //A
	int16_t Roll;                         //-1000 -> 1000
	int16_t Yaw;                          //-1000 -> 1000
	int16_t TX;                           //-1000 -> 1000 //B
	int16_t TY;                           //-1000 -> 1000
	int16_t TZ;                           //-1000 -> 1000 
	int16_t Throttle;                     // 0 -> 1000    //C
	int16_t WheelSteer;                   //-1000 -> 1000 //D
	int16_t WheelThrottle;                // 0 -> 1000
	float targetHeading, targetPitch, targetRoll;        //E
	uint8_t SASMode; //hold, prograde, retro, etc...
	uint8_t SpeedMode; //Surface, orbit target
	uint8_t timeWarpRateIndex;
	
//helper methods
void SetControlerMode(int controler, int mode) {
	switch (controler) {
	case CONTROLLER_ROT:
		 ControlerMode =  ControlerMode & 0b11111100 | mode << (2 * 0);
		break;
	case CONTROLLER_TRANS:
		 ControlerMode =  ControlerMode & 0b11110011 | mode << (2 * 1);
		break;
	case CONTROLLER_THROTTLE:
		 ControlerMode =  ControlerMode & 0b11001111 | mode << (2 * 2);
		break;
	case CONTROLLER_WHEEL:
		 ControlerMode =  ControlerMode & 0b00111111 | mode << (2 * 3);
		break;
	}
}
void ReSetSASHoldVector() {
	targetHeading = targetPitch = targetRoll = NAN;
}
void SetSASHoldVector(float pitch, float heading, float roll) {
	 targetHeading = heading;
	 targetPitch = pitch;
	 targetRoll = roll;
}

void  InputRot(float pitch, float yaw, float roll) {
	 Pitch = (int16_t)pitch;
	 Roll = (int16_t)roll;
	 Yaw = (int16_t)yaw;
}

void  InputTran(float tx, float ty, float tz) {
	 TX = (int16_t)tx;
	 TY = (int16_t)ty;
	 TZ = (int16_t)tz;
}

void  InputThrottle(float throttle) {
	 Throttle = (int16_t)throttle;
}
void	SetMainControl(int control, bool s) {
	if (s) {
		 MainControls |= control;
	}
	else {
		 MainControls &= ~((uint8_t)control);
	}
}
void  SetActionGroup(int group, bool s) {
	if (s) {
		 ActionGroups |= group;
	}
	else {
		 ActionGroups &= ~((uint8_t)group);
	}
}
};

struct StatusPacket {
	//Header h; //Implied 
	int32_t ID;
	int8_t inFlight;
	char vesselName[32];
};

struct VesselPacket {
	//Header h; //Implied 
	uint32_t ID;

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

	bool GetActionGroup(int group) {
		return (ActionGroups & group) != 0;
	}

	bool GetMainControl(int control) {
		return (MainControls & control) != 0;
	}
};

#pragma pack(pop)