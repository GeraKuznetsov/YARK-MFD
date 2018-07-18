#include <iostream>
#include "TCPClient.h"

TCPClient* client;

#pragma pack(push, 1)
struct DataIn {
	char HEADER_0;
	char HEADER_1;
	int ID;
	char status; //0 = 
	float Roll;
	float Pitch;
	float Heading;
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

DataIn* dataIn = new DataIn();

void clientCallback(char data[], TCPClient* client) {
	memcpy(dataIn, data, sizeof(DataIn));

	if (data[0] == (char)0xDE && data[1] == (char)0xAD) {
		std::cout << "data::" << "\n";
		if (dataIn->status) {
			std::cout << "flight\n";
		}
		else {
			std::cout << "ground\n";
		}
		std::cout << "ID: " << dataIn->ID;
		std::cout << "pitch: " << dataIn->Pitch << "\n";
	}
	else {
		std::cout << (int)data[0];
		std::cout << "\n";
		std::cout << (int)data[1];
		std::cout << "\n";
		std::cout << "BAD \n";
	}
}

int main() {
	client = new TCPClient("localhost", "9999", &clientCallback);
	std::cout << "Start:\n";
	while (client->state == TCPCLIENT_CONNECTING);
	if (client->state == TCPCLIENT_FAILED) {
		std::cout << "failed\n";
	}
	while (client->state == TCPCLIENT_CONNECTED) {

	}
	int foo;
	std::cin >> foo;
	return 0;

}