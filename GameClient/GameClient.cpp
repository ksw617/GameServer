#include "pch.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAData wsaData;

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup 에러\n");
		return 1;
	}

	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("소켓 에러 %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(27015);

	if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("Connect 소켓 에러 %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Connected to Server\n");

	while (true)
	{
	
		char recvBuffer[512];
		int32 size = recv(connectSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (size <= 0)
		{
			printf("recv 에러 %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		printf("Receive data : %s[%dbyte]\n", recvBuffer, size);

		char sendBuffer[100] = "Hello This is Client!";
		if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			printf("send 에러 %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		if (GetAsyncKeyState(VK_RETURN))
		{
			shutdown(connectSocket, SD_BOTH);
			break;
		}

		Sleep(1000);
	}

	closesocket(connectSocket);

	WSACleanup();
	return 0;
}