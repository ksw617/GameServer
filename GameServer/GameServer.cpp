#include "pch.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

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

	//Select 모델
	//호출이 성공할 시점을 알수 있음.
	//blocking : 조건이 만족되지 않아서 블록킹되는 상황 예방
	//NonBlocking : 만족되지 않았을때 불필요 반복 체크 하는거 방지 

	fd_set reads;	//읽기용
	fd_set writes;	//쓰기용

	while (true)
	{
		//FD_ZERO : 초기화를 시켜줌
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		//FD_SET : 소켓과 연결
		FD_SET(listenSocket, &reads);

		//timeval timeout : nullptr(무한대기)
		//timeval timeout;
		//timeout.tv_sec; // 초
		//timeout.tv_usec; // 마이크로 초
		if (select(0, &reads, &writes, nullptr, nullptr) == SOCKET_ERROR)
		{
			//오류 발생시 나가기
			break;
		}
		
		//FD_ISSET : 소켓이 set에 들어가 있으면 0이 아닌 값을 리턴
		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
			if (acceptSocket != INVALID_SOCKET)
			{
				printf("Client Connected");
				
				while (true)
				{
					//FD_SET : 소켓과 연결
					FD_SET(acceptSocket, &reads);

					//읽을 준비가 되어 있는지
					if (FD_ISSET(acceptSocket, &reads))
					{
						char recvBuffer[512];
						//받기
						int32 recvLen = recv(acceptSocket, recvBuffer, sizeof(recvBuffer), 0);
						//받은 데이터가 문제가 있을경우
						if (recvLen <= 0)
						{
							//다시 처음부터 시작
							continue;
						}

						//받은 데이터 확인
						printf("Recv Data : %s\n", recvBuffer);
						

						while (true)
						{
							//FD_SET : 소켓과 연결
							FD_SET(acceptSocket, &writes);

							//쓸 준비가 되어 있는지
							if (FD_ISSET(acceptSocket, &writes))
							{
								char sendBuffer[100] = "Hello This is Server!";
								//보내기
								int32 sendLen = send(acceptSocket, sendBuffer, sizeof(sendBuffer), 0);
								//보낸 데이터가 문제가 있을경우
								if (sendLen == SOCKET_ERROR)
								{
									//다시 처음부터 시작
									continue;
								}

								//보낸 데이터 크기 확인
								printf("Send Buffer Length : %d byte\n", sizeof(sendBuffer));
								break;

							}

						}
					}

				}

			}
		}

	}

	closesocket(listenSocket);

	WSACleanup();
	return 0;
}