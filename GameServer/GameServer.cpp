#include "pch.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[512] = { 0 };
	int recvLen = 0;
	int sendLen = 0;
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

	vector<Session> sessions;

	fd_set reads;
	fd_set writes;

	while (true)
	{
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		for (Session& s : sessions)
		{
			FD_SET(s.socket, &reads);
			FD_SET(s.socket, &writes);
			
		}

		FD_SET(listenSocket, &reads);

		int value = select(0, &reads, &writes, nullptr, nullptr);

		if (value == SOCKET_ERROR)
		{
			//에러 발생
			break;
		}

		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
			sessions.push_back(Session{acceptSocket});
			printf("Client Connected..\n");

		}

		for (Session& s : sessions)
		{
			if (FD_ISSET(s.socket, &reads))
			{
				int recvLen = recv(s.socket, s.recvBuffer, sizeof(s.recvBuffer), 0);
				if (recvLen <= 0)
				{
					//sockets에서 제거
					continue;
				}

				printf("Recv Data : %s\n", s.recvBuffer);
				s.recvLen = recvLen;
			}

			if (FD_ISSET(s.socket, &writes))
			{
				int sendLen = send(s.socket, &s.recvBuffer[s.sendLen], s.recvLen - s.sendLen, 0);
				if (sendLen == SOCKET_ERROR)
				{
					//sockets에서 제거
					continue;
				}

				s.sendLen += sendLen;
				if (s.recvLen == s.sendLen)
				{
					s.recvLen = 0;
					s.sendLen = 0;
				}
			}
			
		}

	}

	

	closesocket(listenSocket);

	WSACleanup();
	return 0;
}