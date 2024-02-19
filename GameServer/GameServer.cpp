#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <vector>


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

	//연결 수락 및 처리를 위한 메인 루프
	while (true)
	{
		SOCKET acceptSocket = INVALID_SOCKET;

		//WSA : Window socket API

		//accept
		while (true)
		{
			//non-blocking 으로 accept을 실행
			acceptSocket = accept(listenSocket, NULL, NULL);

			//acceptSocket이 INVALID_SOCKET 이라면
			if (acceptSocket == INVALID_SOCKET)
			{
				//아직 들어오는 연결이 없음
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					//받을때 까지 continue
					continue;
				}

				//그게 아니면 여기는 에러니까.
				//프로그램 종료
				closesocket(listenSocket);
				WSACleanup();
				return 1;

			}
			else
			{
				//클라 접속
				printf("Client Connected\n");
				break;
			}
		}

	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

