#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include "TCPClient.h"

int TCPClient::ClientSend(char sendbuff[]) {
	int	iResult = send(ConnectSocket, sendbuff, (int)strlen(sendbuff), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

void TCPClient::TCPClientRun(std::string IP, std::string PORT, void(*callback_proccess)(char[], TCPClient*)) {
	WSADATA wsaData;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	int iResult;

	// Validate the parameters

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		error = "WSAStartup failed";
		state = TCPCLIENT_FAILED;
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(IP.c_str(), PORT.c_str(), &hints, &result);
	if (iResult != 0) {
		error = "getaddrinfo failed";
		state = TCPCLIENT_FAILED;
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			error = "INVALID_SOCKET";
			state = TCPCLIENT_FAILED;
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			error = "SOCKET_ERROR";
			//SERVER_LIST_PROMPT = SERVER_LIST_PROMPT_ERROR;
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		error = "Unable to connect to server!";
		state = TCPCLIENT_FAILED;
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}

	std::cout << "cleint in revieve loop\n";
	
	//state = TCPCLIENT_CONNECTED;

	Running = true;
	do {
		char recvbuf[256];
		iResult = recv(ConnectSocket, recvbuf, 256, 0);
		if (iResult > 0) {
			/*printf("Bytes received: %d\n", iResult);
			for (int i = 0; i < iResult; i++) {
				std::cout << (int)recvbuf[i] << " : " ;
			}
			std::cout << "\n";*/
			callback_proccess(recvbuf, this);
		}
		else if (iResult == 0) {
			error = "recv failed";
			//SERVER_LIST_PROMPT = SERVER_LIST_PROMPT_ERROR;
			state = TCPCLIENT_FAILED;
			printf("Connection closed\n");
		}
		else {
			error = "con closed";
			//SERVER_LIST_PROMPT = SERVER_LIST_PROMPT_ERROR;
			state = TCPCLIENT_FAILED;
			printf("recv failed with error: %d\n", WSAGetLastError());
		}
	} while (iResult > 0 && Running);
	if (Running) {
		Shutdown();
	}
}

TCPClient::TCPClient(std::string IP, std::string PORT, void(*callback_proccess)(char[], TCPClient*))
{
	recLoop = std::thread(&TCPClient::TCPClientRun, this, IP, PORT, callback_proccess);
	recLoop.detach();
}

void TCPClient::Shutdown() {
	state = TCPCLIENT_FAILED;
	std::cout << "thread shutdown\n";
	Running = false;
	if (shutdown(ConnectSocket, SD_SEND) == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}
}
