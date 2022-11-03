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

	//생성 : WSACreateEvent
	//소멸 : WSACloseEvent
	//준비 되면 : WSAWaitForMultipleEvents
	//어떤 애인지 알려면 : WSAEnumNetworkEvents

	//클라
	//FD_CONNECT : 연결 절차 완료
	//FD_READ : 데이터 수신 가능 recv
	//FD_WRITE : 데이터 송신 가능 send
	//FD_CLOSE : 상대가 접속 종료

	//서버
	//FD_ACCEPT : 접속한 클라가 있음 accept
	//FD_READ : 데이터 수신 가능 recv
	//FD_WRITE : 데이터 송신 가능 send
	//FD_CLOSE : 상대가 접속 종료

	
	closesocket(listenSocket);

	WSACleanup();
	return 0;
}