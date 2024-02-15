#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <vector>

//구조체 하나 만들기
struct Session
{
	SOCKET sock = INVALID_SOCKET;
	char recvBuffer[512] = {};
	int recvLen = 0;
	int sendLen = 0;

};

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

	//IPv4 & TCP
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

	printf("listening...\n");

	//Session을 담을 vector
	vector<Session> sessions;

	fd_set reads;	//읽기 전용
	fd_set writes;  //쓰기 전용


	while (true)
	{
		//초기화
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		//담은 session들 순회
		for (Session& session : sessions)
		{
			//크기 비교해서
			//0					  0
			if (session.recvLen <= session.sendLen)
			{
				//읽고
				FD_SET(session.sock, &reads);
			}
			else
			{	
				//쓰고
				FD_SET(session.sock, &writes);
			}
		
		}

		FD_SET(listenSocket, &reads);

		//writes도 체크
		if(select(0, &reads, &writes, nullptr, nullptr) == SOCKET_ERROR)
		{
			printf("select failed with error : %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}


		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
			//Session 하나 만들고
			Session session;
			//해당 구조체 SOCKET 에다가 접속한 소켓을 넣어주고
			session.sock = acceptSocket;
			//sessions에 밀어 넣음
			sessions.push_back(session);
			printf("Client Connected...\n");
		}

		for (Session& session : sessions)
		{
			if (FD_ISSET(session.sock, &reads))
			{

				int recvLen = recv(session.sock, session.recvBuffer, sizeof(session.recvBuffer), 0);

				if (recvLen <= 0)
				{
					continue;
				}

				session.recvLen = recvLen;
				printf("Recv Data : %s\n", session.recvBuffer);
			}

			//writes에 해당 socket이 있다면
			if (FD_ISSET(session.sock, &writes))
			{

				//받은거 바로 보내기
				int sendLen = send(session.sock, session.recvBuffer, sizeof(session.recvBuffer), 0);


				//이미 준비되어 있으니까 보내버림
				if (sendLen == SOCKET_ERROR)
				{
					continue;
				}

				session.sendLen = sendLen;
				if (session.recvLen == session.sendLen)
				{
					session.recvLen = 0;
					session.sendLen = 0;
				}

				//얼마나 보냈는지 확인
				printf("Send Buffer Length : %d byte \n", sendLen);
			}

		}

	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

