#include "Serial.h"
#if ENABLE_ARDUINO 
SerialPort::SerialPort(char* portName)
{
	connected = false;

	handler = CreateFileA(static_cast<LPCSTR>(portName), GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //FILE_FLAG_OVERLAPPED //FILE_ATTRIBUTE_NORMAL
	if (handler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			printf("ERROR: port %s not available\n", portName);
		}
		else
		{
			printf("ERROR!!!");
		}
	}
	else {
		COMMTIMEOUTS timeouts = { 0, //interval timeout. 0 = not used
			  0, // read multiplier
			 10, // read constant (milliseconds)
			  0, // Write multiplier
			  0  // Write Constant
		};


		SetCommTimeouts(this->handler, &timeouts);

		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(handler, &dcbSerialParameters)) {
			printf("failed to get current serial parameters");
		}
		else {
			dcbSerialParameters.BaudRate = ARDUINO_BAUD;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(handler, &dcbSerialParameters))
			{
				printf("ALERT: could not set Serial port parameters\n");
			}
			else {
				connected = true;
				printf("Success: connected on port: %s\n", portName);
				PurgeComm(handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}
}

SerialPort::~SerialPort()
{
	if (connected) {
		connected = false;
		CloseHandle(handler);
	}
}

#include <chrono>

bool SerialPort::readBytes(unsigned char* buffer, unsigned int numBytes)
{
	int read = 0;
	DWORD bytesRead;
	ClearCommError(this->handler, &this->errors, &this->status);
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (read < numBytes) {
		ReadFile(this->handler, buffer + read, numBytes - read, &bytesRead, NULL);
		read += bytesRead;
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() > ARDUINO_TIME_OUT) {
			printf("read timeout\n");
			return false;
		}
	}

	return true;
}

bool SerialPort::writeBytes(unsigned char* buffer, unsigned int buf_size)
{
	DWORD bytesSend;
	if (!WriteFile(handler, (void*)buffer, buf_size, &bytesSend, 0)) {
		ClearCommError(handler, &errors, &status);
		return false;
	}
	else return true;
}

bool SerialPort::isConnected()
{
	return connected;
}
#endif
