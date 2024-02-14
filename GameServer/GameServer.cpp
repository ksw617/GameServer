#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <vector>

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

	//접속한 소켓을 담을 구조체
	vector<SOCKET> sockets;

	fd_set reads;	//읽기 전용
	fd_set writes;  //쓰기 전용

	//Socket sock;
	//fd_set set;
	//FD_ZERO : 빈 집합을 초기화. ex) FD_ZERO(&set);
	//FD_CLR  : 집합에서 소켓을 제거. ex) FD_CLR(sock, &set); // 해당 소켓을 해당 집합에서 제거
	//FD_ISSET: 해당 소켓이 집합의 맴버인지 확인, set에 들어가 있으면 TRUE를 반환, ex) FD_ISSET(sock, &set);
	//FD_SET  : 소켓을 추가. ex) FD_SET(sock, &set); //해당 set에 sock 추가

	while (true)
	{

		FD_ZERO(&reads);

		//이미 접속해서 통신할수 있는 애들 하나씩 순회
		for (SOCKET& sock : sockets)
		{
			//reads에 등록
			FD_SET(sock, &reads);
		}

		FD_SET(listenSocket, &reads);

		printf("Before\n");

		//등록된 애들중에 send를 했거나, 새로운 클라가 접속(connect)을 했거나
		if(select(0, &reads, nullptr, nullptr, nullptr) == SOCKET_ERROR)
		{
			printf("select failed with error : %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}


		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
			//sockets에다가 접속된 client와 통신할 수 있는 acceptsSocket을 추가
			sockets.push_back(acceptSocket);
			printf("Client Connected...\n");
		}

		//이미 접속해서 통신할수 있는 애들 하나씩 순회
		for (SOCKET& sock : sockets)
		{
			//해당 소켓이 무언가 보냈다면 reads에 등록 되어 있을꺼고
			//FD_ISSET은 해당 소켓이 등록되어 있는지 없는지만 체크
			if (FD_ISSET(sock, &reads))
			{
				char recvBuffer[512];

				//Non blocking recv <- 여기 까지 왔다는건 select 넘어와서 reads에 등록되어 있으니
				//준비가 되어 있는 상태
				int recvLen = recv(sock, recvBuffer, sizeof(recvBuffer), 0);

				//보냈긴 했는데 값이 없으면
				if (recvLen <= 0)
				{
					//해당 소켓을 날림
					sockets.erase(remove(sockets.begin(), sockets.end(), sock), sockets.end());
					continue;
				}

				//받은 데이터 체크
				printf("Recv Data : %s\n", recvBuffer);
			}

		}

	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

