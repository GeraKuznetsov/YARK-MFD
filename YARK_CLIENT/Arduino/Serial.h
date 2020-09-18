#pragma once
//gives time for arduino to reset, run bootloader, etc
#include "EnableArduino.h"
#if ENABLE_ARDUINO 
#define ARDUINO_WAIT_TIME 4000
#define ARDUINO_BAUD CBR_115200
#define ARDUINO_TIME_OUT 1000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class SerialPort
{
private:
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;
public:
	SerialPort(char* portName);
	~SerialPort();
	bool readBytes(unsigned char* buffer, unsigned int buf_size);
	bool writeBytes(unsigned char* buffer, unsigned int buf_size);
	bool isConnected();
};
#endif
