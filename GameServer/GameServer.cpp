#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <thread> 

struct Session
{
	WSAOVERLAPPED overlapped = {};	//비동기 I/O 작업을 위한 구조체
	SOCKET socket = INVALID_SOCKET; // 클라이언트와의 통신을 담당하는 소켓
	char recvBuffer[512] = {};		// 데이터 수신을 위한 버퍼

};



void RecvThread(HANDLE iocpHandle)
{
	DWORD byteTransferred = 0;
	ULONG_PTR key = 0;
	Session* session = nullptr;

	while (true)
	{
		printf("Waiting....\n");

		if (GetQueuedCompletionStatus(iocpHandle, &byteTransferred, &key, (LPOVERLAPPED*)&session, INFINITE))
		{
			printf("recv Data : %s\n", session->recvBuffer);

			//수신 버퍼 및 기타 정보를 설정하여 다시 데이터 수신 준비
			WSABUF wsaBuf;
			wsaBuf.buf = session->recvBuffer;			//수신 버퍼 지정
			wsaBuf.len = sizeof(session->recvBuffer);	//버퍼의 크기 지정

			DWORD recvLen = 0;		//수신된 데이터 길이를 저장할 변수
			DWORD flags = 0;		//flags 현재 안사용함

			//비동기 수신을 다시 시작. 지속적인 데이터 수신을 위해 반복
			WSARecv(session->socket, OUT & wsaBuf, 1, OUT & recvLen, OUT & flags, &session->overlapped, NULL);
		}

	}
}


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

	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	thread t(RecvThread, iocpHandle);

	while (true)
	{
		SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			closesocket(listenSocket); 
			WSACleanup();
			return 1;

		}

		printf("Client Connected\n");
		
		ULONG_PTR key = 0;
		CreateIoCompletionPort((HANDLE)acceptSocket, iocpHandle, key, 0);


		Session* session = new Session;
		session->socket = acceptSocket;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = sizeof(session->recvBuffer);

		DWORD recvLen = 0;
		DWORD flags = 0;

		WSARecv(acceptSocket, OUT & wsaBuf, 1, OUT & recvLen, OUT & flags, &session->overlapped, NULL);

	}

	t.join();

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

