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

	//TCP
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

	while (true)
	{
		SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			//아직 받지 않음
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}

			//진짜 에러
			break;

		}

		printf("CLient Connected\n");

		while (true)
		{
			char recvBuffer[512];
			int recvLen = recv(acceptSocket, recvBuffer, sizeof(recvBuffer), 0);
			 //소켓 에러일 경우
			if (recvLen == SOCKET_ERROR)
			{
				//클라에서 보내질 않은거지 문제는 없음
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					//Recv -> while 
					//보냈는지 계속 체크
					continue;

				}

				//진짜 에러
				break;

			}
			else if (recvLen == 0) //보내데이터가 0byte
			{
				//연결 끊음
				break;
			}

			//받은 데이터 확인
			printf("Recv Data : %s\n", recvBuffer);
		}
	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

