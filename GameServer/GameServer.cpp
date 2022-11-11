#include "pch.h"

#include <thread>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[512] = {};
	int32 recLen = 0;
};

enum IO_TYPE
{
	IO_NONE,
	IO_RECV,
	IO_SEND,
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	IO_TYPE type;
};

void RecvThread(HANDLE iocpHandle)
{
	

	while (true)
	{
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;


		BOOL returnValue = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, (ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);
		if (returnValue == FALSE || bytesTransferred == 0)
		{
			continue;
		}

		overlappedEx->type = IO_TYPE::IO_RECV;
		printf("recv Length  %d\n", bytesTransferred);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = sizeof(session->recvBuffer);

		DWORD recvLen = 0;
		DWORD flags = 0;

		WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, nullptr);
	}
}

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

	vector<Session*> sessionManager;
	HANDLE iocpHandle =  CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	
	thread t(RecvThread, iocpHandle);

	//TODO
	while (true)
	{
		SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket ==INVALID_SOCKET)
		{
			//오류
			continue;
		}
		printf("Client Connected!\n");

		Session* session = new Session();
		session->socket = acceptSocket;
		sessionManager.push_back(session);

		CreateIoCompletionPort((HANDLE)acceptSocket, iocpHandle, (ULONG_PTR)session, NULL);
		
		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::IO_RECV;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = sizeof(session->recvBuffer);

		DWORD recvLen = 0;
		DWORD flags = 0;

		WSARecv(acceptSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, nullptr);

		printf("recv Length  %d\n", recvLen);
	}

	t.join();

	WSACleanup();
	return 0;
}