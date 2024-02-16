#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <vector>

//구조체 하나 만들기
struct Session
{
	SOCKET sock = INVALID_SOCKET;
	char recvBuffer[512] = {};
	int recvLen = 0;
	int sendLen = 0;

};

int main()
{
	printf("============= SERVER =============\n");

	WORD wVersionRequested;
	WSAData wsaData;
	
	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return 1;
	}

	//IPv4 & TCP
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
							  
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	
	u_long iMode = 1;
	if (ioctlsocket(listenSocket, FIONBIO, &iMode) == INVALID_SOCKET)
	{
		printf("ioctlsocket failed with error : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSAGetLastError();
		return 1;
	}

	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(27015);

	if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) 
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}

	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSAGetLastError();
		return 1;
	}

	printf("listening...\n");

	vector<SOCKET> sockets;
	vector<WSAEVENT> wsaEvents;
	sockets.push_back(listenSocket);

	WSAEVENT listenEvent = WSACreateEvent();
	wsaEvents.push_back(listenEvent);


	if (WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		printf("WSAEventSelect failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	};

	while (true)
	{
		DWORD index = WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
		
		if (index == WSA_WAIT_FAILED)
		{
			continue;
		}

		index -= WSA_WAIT_EVENT_0;

		WSANETWORKEVENTS networkEvents;

		if (WSAEnumNetworkEvents(sockets[index], wsaEvents[index], &networkEvents) == SOCKET_ERROR)
		{
			continue;
		}

#pragma region Accept
		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{

			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				continue;
			}

			SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
			if (acceptSocket != INVALID_SOCKET)
			{
				printf("Client Connected...\n");

				sockets.push_back(acceptSocket);

				WSAEVENT acceptEvent = WSACreateEvent();

				wsaEvents.push_back(acceptEvent);

				if (WSAEventSelect(acceptSocket, acceptEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
				{
					printf("WSAEventSelect failed with error %d\n", WSAGetLastError());
					closesocket(acceptSocket);
					closesocket(listenSocket);
					WSACleanup();
					return 1;
				}
			}
		}
#pragma endregion

#pragma region READ

		//acceptsocket들을 체크
		//해당 소켓이 FD_READ 할 준비가 끝났다면
		if (networkEvents.lNetworkEvents & FD_READ)
		{
			//에러 발생시
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
			{
				//다시 루프
				continue;
			}
		}

		//이벤트가 발생한 해당소켓을 참조
		SOCKET& sock = sockets[index];

		//데이터 받을 버퍼
		char recvBuffer[512] = {};
		//준비가 되어 있으니 바로 받음 됨
		int recvLen = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
		//에러 발생시
		if (recvLen == SOCKET_ERROR)
		{
			//WSAEWOULDBLOCK 상태가 아니면 문제가 있는거니까
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				//다시 루프
				continue;
			}

		}

		//받은 데이터 확인
		printf("Recv Data : %s\n", recvBuffer);
		

#pragma endregion


	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

