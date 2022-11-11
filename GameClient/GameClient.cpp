#include "pch.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <thread>
using namespace this_thread;


int main()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAData wsaData;

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup 에러\n");
		return -1;
	}

	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("소켓 에러 %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}




	u_long on = 1;
	if (ioctlsocket(connectSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		printf("ioctlsocket 에러 %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return -1;
	}


	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(27015);

	while (true)
	{
		if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			if (WSAGetLastError() == WSAEALREADY)
			{
				continue;
			}

			if (WSAGetLastError() == WSAEISCONN)
			{
				break;
			}

			printf("connect 에러 %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return -1;
			
		}
	}

	printf("Connected To server!\n");
	
	char sendBuffer[100] = "Hello This is Client!";
	WSAOVERLAPPED overlaped = {};
	WSAEVENT wsaEvent = WSACreateEvent();
	overlaped.hEvent = wsaEvent;
	

	while (true)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = sendBuffer;
		wsaBuf.len = sizeof(sendBuffer);

		DWORD sendLen = 0;
		DWORD flags = 0;

		if (WSASend(connectSocket, &wsaBuf, 1, &sendLen, flags, &overlaped, nullptr) == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSA_IO_PENDING)
			{
				//기다림
				WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
				if (WSAGetOverlappedResult(connectSocket, &overlaped, &sendLen, FALSE, &flags))
				{
					printf("Overlapped Result : ");
				}
			}
			else
			{
				//문제 있는 상황
				break;
			}
		}

		printf("Send Buffer Length : %d byte\n", sizeof(sendBuffer));

		sleep_for(1s);
	}

	WSACloseEvent(wsaEvent);
	
	closesocket(connectSocket);
	WSACleanup();
	return 0;

}