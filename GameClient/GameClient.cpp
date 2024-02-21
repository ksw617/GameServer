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

		//연결 실패 시 오류 메세지 출력
		printf("connect failed with error %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;

	}
	

	//연결 성공
	printf("Connected\n");

	//보낼 버퍼 할당. 값 초기화
	char sendBuffer[] = "Hello this is client!";

	while (true)
	{
		//데이터 보내기
	//만약에 소켓 에러라면
		if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			//에러 코드 확인
			printf("Send Error %d\n", WSAGetLastError());

			//소켓 닫고
			closesocket(connectSocket);

			//클린업 하고
			WSACleanup();

			//프로그램 종료
			return 1;
		}

		//얼마나 보냈는지 체크
		printf("Send Buffer : %d bytes\n", sizeof(sendBuffer));

		Sleep(1000);
	}


	closesocket(connectSocket);
	WSACleanup();
}

