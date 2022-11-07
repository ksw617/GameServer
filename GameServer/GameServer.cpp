#include "pch.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//IN DWORD dwError,
//IN DWORD cbTransferred,
//IN LPWSAOVERLAPPED lpOverlapped,
//IN DWORD dwFlags

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
{
	printf("Call back Recv Length : %d\n", recvLen);
}

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

	//Todo

	while (true)
	{
		SOCKET acceptSocket = INVALID_SOCKET;
		
		while (true)
		{
			
			acceptSocket = accept(listenSocket, NULL, NULL);
			if (acceptSocket == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}

				//진짜 에러
				return -1;

			}
			else
			{
				printf("Client Connected\n");
				break;
			}
		}

		WSAOVERLAPPED overlapped = {};

		while (true)
		{
			char recvBuffer[512];
			WSABUF wsaBuf;
			wsaBuf.buf = recvBuffer;
			wsaBuf.len = sizeof(recvBuffer);
			
			DWORD recvLen = 0;
			DWORD flags = 0;

			if (WSARecv(acceptSocket, &wsaBuf, 1, &recvLen, &flags, &overlapped, RecvCallback) == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSA_IO_PENDING)
				{
					//기다림
					SleepEx(INFINITE, TRUE);
				}
				else
				{
					break;
				}
			}

			printf("Recv Data : %s\n", recvBuffer);

		}

		closesocket(acceptSocket);
		WSACloseEvent(wsaEvent);


	}

	closesocket(listenSocket);

	WSACleanup();
	return 0;
}