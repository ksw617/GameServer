#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

int main()
{
	WORD wVersionRequested;
	WSAData wsaData;
	
	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return 1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
							  
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error %d\n", WSAGetLastError());
		return 1;
	}

	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(27015);

	if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind failed with error %d", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}


	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen failed with error %d", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	while (true)
	{
		printf("listening...\n");


		 SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		 //해당 소켓이 유효하지 않다면
		 if (acceptSocket == INVALID_SOCKET)
		 {
			 //에러 코드 확인
			 printf("accept function failed with error %d\n", WSAGetLastError());
			 //소켓 닫아주고
			 closesocket(listenSocket);
			 //Winsock 사용 종료
			 WSACleanup();
			 //프로그램 종료
			 return 1;
		 }

		 //연결성공
		 printf("Client connected.\n");

	}


	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

