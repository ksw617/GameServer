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
		
		//보낼 버퍼 값 넣어주고
		char sendBuffer[] = "Hello this is Client!";

		//이미 준비되어 있으니까 보내버림
		if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			//연결 닫음.
			closesocket(connectSocket);
			WSACleanup();
			return 1;

		}

		//얼마나 보냈는지 확인
		printf("Send Buffer Length : %d byte \n", sizeof(sendBuffer));


		Sleep(1000);
	}


	closesocket(connectSocket);
	WSACleanup();
}

