#include "pch.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <thread>
using namespace this_thread;


int main()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAData wsaData;

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup 에러\n");
		return -1;
	}

	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("소켓 에러 %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	u_long on = 1;
	if (ioctlsocket(connectSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		printf("ioctlsocket 에러 %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return -1;
	}


	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(27015);

	while (true)
	{
		if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			//즉시 완료할 수 없는 비블로킹 소켓의 작업에서 반환
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			//작업이 이미 진행 중
			if (WSAGetLastError() == WSAEALREADY)
			{
				continue;
			}

			//소켓이 이미 연결되어 있음
			if (WSAGetLastError() == WSAEISCONN)
			{
				break;
			}

			printf("connect 에러 %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return -1;
			
		}
	}

	printf("Connected To server!\n");
	

	while (true)
	{

		//Send
		char sendBuffer[100] = "Hello!";
		while (true)
		{
			if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}

				//진짜 에러
				return -1;
			}

			printf("Send Buffer Length : %d byte\n", sizeof(sendBuffer));
			break;
		}

		////Recv
		//while (true)
		//{
		//	char recvBuffer[512];
		//	int32 recvLen = recv(connectSocket, recvBuffer, sizeof(recvBuffer), 0);
		//	if (recvLen == SOCKET_ERROR)
		//	{
		//		if (WSAGetLastError() == WSAEWOULDBLOCK)
		//		{
		//			continue;
		//		}
		//
		//		//진짜 에러
		//		break;
		//	}
		//	else if (recvLen == 0)
		//	{
		//		//연결 끊김
		//		break;
		//	}
		//
		//	//받은 데이터 확인
		//	printf("Recv Data : %s\n", recvBuffer);
		//	break;
		//}
		

		sleep_for(1s);
	}

	
	closesocket(connectSocket);
	WSACleanup();
	return 0;

}