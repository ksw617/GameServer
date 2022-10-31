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

	//Accept
	while (true)
	{
		SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			//진짜에러
			break;
		
		}

		printf("Client Connected\n");

		//Recv
		while (true)
		{
			char recvBuffer[512];
			int32 recvLen = recv(acceptSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}

				//진짜 에러
				break;
			}
			else if (recvLen == 0)
			{
				//연결 끊김
				break;
			}

			printf("Recv Data : %s\n", recvBuffer);

			//Send
			char sendBuffer[100] = "Hello this is server!";
			while (true)
			{
				if (send(acceptSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
						continue;
					}

					//진짜 에러
					break;
				}

				//보낸 데이터 크기 확인
				printf("Send Buffer Length : %d byte\n", sizeof(sendBuffer));
				break;
			}
		}
	
	}
	


	closesocket(listenSocket);

	WSACleanup();
	return 0;
}