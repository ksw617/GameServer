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

	//연결 수락 및 처리를 위한 메인 루프
	while (true)
	{
		SOCKET acceptSocket = INVALID_SOCKET;

		//WSA : Window socket API

		//accept
		while (true)
		{

			acceptSocket = accept(listenSocket, NULL, NULL);

			if (acceptSocket == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}

				closesocket(listenSocket);
				WSACleanup();
				return 1;

			}
			else
			{
			  	printf("Client Connected\n");
				break;
			}
		}



		//WSARecv
		//s : 소켓. 위에 acceptSocket 넣어 주면됨
		//lpBuffers : WSABUF 구조체 배열의 대한 포인터			 
		//dwBufferCount : WSABUF 구조체 갯수
		//lpNumberOfBytesRecvd : 수신 완료 후 받은 바이트에 대한 포인터
		//lpFlags : 플래그
		//lpOverlapped : WSAOVERLAPPED 구조체 대한 포인터
		//lpCompletionRoutine : 수신 완료후 호출되는 콜백함수

		WSAOVERLAPPED overlapped = {};			//WSAOVERLAPPED 구조체 할당후 초기화. 비동기 I/O 작업에 사용
		WSAEVENT wsaEvent = WSACreateEvent();	//비동기 작업을 위한 이벤트 객체 생성
		overlapped.hEvent = wsaEvent;			//WSAOVERLAPPED 구조체에 있는 hEvent에 객체 연결

		char recvBuffer[512];					//데이터 수신을 위한 버퍼
		   
		//클라이언트로부터 데이터를 지속적으로 수신
		while (true)
		{
			WSABUF wsaBuf;						//WSABUF 구조체 선언. WSARecv 함수에 사용될 버퍼를 관리
			wsaBuf.buf = recvBuffer;			//버퍼 포인터를 recvBuffer로 설정
			wsaBuf.len = sizeof(recvBuffer);	//버퍼 크기 설정

			DWORD recvLen = 0;					//수신된 데이터 길이를 저장할 변수
			DWORD flags = 0;					//플래그 변수, 현재는 사용하지 않음

			//WSARecv 함수를 이용해 비동기적으로 데이터를 수신 요청
			if (WSARecv(acceptSocket, &wsaBuf, 1, &recvLen, &flags, &overlapped, nullptr) == SOCKET_ERROR)
			{
				//Recv 작업이 성공적으로 시작되었으며 나중에 완료될 경우
				if (WSAGetLastError() == WSA_IO_PENDING)
				{
					//수신 작업이 완료될 때까지 이벤트 대기
					WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
					//완료된 비동기 작업을 가져옴. 이 과정에서 실제로 데이터가 수신됨
					WSAGetOverlappedResult(acceptSocket, &overlapped, &recvLen, FALSE, &flags);

				}
				else//SOCKET_ERROR인 상태에서 WSA_IO_PENDING이 아니라면 진짜 에러
				{
					//수신 루프 빠져 나가기
					break;
				}

			}

			//수신된 데이터 출력. recvBuffer에 저장된 내용을 화면에 표시
			printf("Recv Data : %s\n", recvBuffer);
		}

		closesocket(acceptSocket);	//해당 소켓은 닫아주고
		WSACloseEvent(wsaEvent);	// 사용된 이벤트 객체를 닫음

	}

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}

