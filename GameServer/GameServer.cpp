#include "pch.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[512] = {};
	int32 recvLen = 0;
	int32 sendLen = 0;

};

int main()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAData wsaData;

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup 에러\n");
		return -1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Listen 소켓 에러 %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	u_long on = 1;
	if (ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		printf("ioctlsocket failed with error : %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(27015);

	if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind failed with error : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}

	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen failed with error : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}

	printf("Listening....\n");

	vector<WSAEVENT> wsaEvents;
	vector<SOCKET> sockets;


	WSAEVENT listenEvent = WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sockets.push_back(listenSocket);

	int32 eventSelect = WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE);
	if (eventSelect == SOCKET_ERROR)
	{
		return -1;
	}

	while (true)
	{
		int eventIndex = WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
		if (eventIndex == WSA_WAIT_FAILED)
		{
			continue;
		}

		eventIndex -= WSA_WAIT_EVENT_0;

		WSANETWORKEVENTS networkEvents;
		if (WSAEnumNetworkEvents(sockets[eventIndex], wsaEvents[eventIndex], &networkEvents) == SOCKET_ERROR)
		{
			continue;
		}

		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				continue;
			}

			//accept
			SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
			if (acceptSocket != INVALID_SOCKET)
			{
				printf("Client Connected\n");

				WSAEVENT acceptEvent = WSACreateEvent();
				wsaEvents.push_back(acceptEvent);
				sockets.push_back(acceptSocket);
				if (WSAEventSelect(acceptSocket, acceptEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
				{
					return -1;
				}
			}
		}

		
		if (networkEvents.lNetworkEvents & FD_READ)
		{
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
			{
				continue;
			}

			SOCKET& sock = sockets[eventIndex];
			
			char recvBuffer[512];
			int recvLen = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					continue;
				}
			}

			printf("Recv Data: %s\n", recvBuffer);
		}

		//보내는거
		if (networkEvents.lNetworkEvents & FD_WRITE)
		{
			//에러 체크
			if (networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
			{
				continue;
			}
			SOCKET& sock = sockets[eventIndex];

			char sendBuffer[100] = "Hello!";
			int sendLen = send(sock, sendBuffer, sizeof(sendBuffer), 0);
			if (sendLen == SOCKET_ERROR)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK)
				{
					continue;
				}
			}

			//보낸 데이터 확인
			printf("Send Buffer Length : %d byte\n", sizeof(sendBuffer));
		}
	}
	
	closesocket(listenSocket);

	WSACleanup();
	return 0;
}