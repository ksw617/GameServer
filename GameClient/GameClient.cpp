#include <iostream>			   

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>


int main()
{
	Sleep(1000);

	printf("============= CLIENT =============\n");

	WORD wVersionRequested;
	WSAData wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup function failed with error\n");
		return 1;
	}

	//IPV4 & TCP
	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;

	}

	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(27015);

	if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("connect failed with error %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;

	}

	printf("Connected\n");

	while (true)
	{
	

		char sendBuffer[512] = "Hello This is Client!";
		if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			printf("Send Error %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
	
		}
	
		printf("Send Buffer : %d bytes\n", sizeof(sendBuffer));



		//char recvBuffer[512];
		//
		//int recvLen = recv(connectSocket, recvBuffer, sizeof(recvBuffer), 0);
		//if (recvLen <= 0)
		//{
		//	printf("Recv Error : %d\n", WSAGetLastError());
		//	closesocket(connectSocket);
		//	WSACleanup();
		//	return 1;
		//}
		//
		//printf("Recv Buffer Data : %s\n", recvBuffer);
		//printf("Recv buffer Length : %d bytes\n", recvLen);



		Sleep(1000);
	
	}

	closesocket(connectSocket);
	WSACleanup();
}

