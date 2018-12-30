/*
#include "Serial.h"
#include <iostream>
#include "Structs.h"

SerialPort *sp;
char port[] = "COM3";
char read[256];

void main() {
	std::cout << "start\n";
	sp = new SerialPort(port);
	if (!sp->isConnected()) {
		return;
	}
	DataOut.test = 0;
	DataOut.HEADER_0 = 0xFF;
	DataOut.HEADER_1 = 0x06;
	for (int i = 0; i < 5; i++) {
		DataOut.test = !DataOut.test;
		sp->writeSerialPort((char*)&DataOut, sizeof(DataOut));
		sp->readSerialPort(read, 256);
		std::cout << read;
		Sleep(1000);
	}
	int foo;
	std::cin >> foo;
}*/