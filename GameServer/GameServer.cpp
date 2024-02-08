#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

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

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
							  
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error %d\n", WSAGetLastError());
		WSACleanup();
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

		SOCKADDR_IN clientService;
		int	addrLen = sizeof(clientService);
		memset(&clientService, 0, addrLen);

		SOCKET acceptSocket = accept(listenSocket, (SOCKADDR*)&clientService, &addrLen);
		 if (acceptSocket == INVALID_SOCKET)
		 {
			 printf("accept function failed with error %d\n", WSAGetLastError());
			 closesocket(listenSocket);
			 WSACleanup();
			 return 1;
		 }

		 printf("Client connected.\n");

		 char ipAddress[16];
		 inet_ntop(AF_INET, &clientService.sin_addr, ipAddress, sizeof(ipAddress));
		 printf("Client connected IP : %s\n", ipAddress);

		 while (true)
		 {
			 char sendBuffer[] = "Hello this is server!";

			 if (send(acceptSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
			 {
				 printf("Send Error %d\n", WSAGetLastError());
				 closesocket(acceptSocket);
				 break;
			 }

			 printf("Send Data : %s\n", sendBuffer);

			 char recvBuffer[512];
			 int recvLen = recv(acceptSocket, recvBuffer, sizeof(recvBuffer), 0);

			 if (recvLen <= 0)
			 {
				 printf("Recv Error : %d\n", WSAGetLastError());
				 closesocket(acceptSocket);
				 break;

			 }

			 printf("Recv Buffer Data[%s] : %s\n", ipAddress, recvBuffer);
			 printf("Recv Buffer Length : %d bytes\n", recvLen);
		 }
	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

