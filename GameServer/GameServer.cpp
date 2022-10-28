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

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Listen 소켓 에러 %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	//inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(27015);

	if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind 에러 %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen 에러 %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	while (true)
	{
		printf("Listening.....\n");

		SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			printf("Accept 소켓 에러 %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		printf("Client Connected\n");

		while (true)
		{
			char sendBuffer[100] = "Hello This is Server!";
			if (send(acceptSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
			{
				printf("send 에러 %d\n", WSAGetLastError());
				closesocket(acceptSocket);
				break;
			}

			char recvBuffer[512];
			int32 size = recv(acceptSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (size <= 0)
			{
				printf("recv 에러 %d\n", WSAGetLastError());
				closesocket(acceptSocket);
				break;
			}

			printf("Receive data : %s[%dbyte]\n", recvBuffer, size);
		}

		
	}

	closesocket(listenSocket);

	WSACleanup();
	return 0;
}