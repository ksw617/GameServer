#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

int main()
{               
    printf("============== Client  ================\n");

    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 2);

    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStarup filed with error\n");
        return 1;
    }

    //소켓 만들기
    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket function failed with error : %d\n", WSAGetLastError());
    }
    else
    {
        printf("socket function succeded\n");
    }

    //서버의 주소
    SOCKADDR_IN service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(27015);

	//TODO

    //서버에 접속 service(접속할 서버 정보)
    //connect(내 소켓, 서버의 주소, 해당 주소의 구조체 크기)
    if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        //에러 코드 확인
        printf("connect function failed with error : %d\n", WSAGetLastError());
        //소켓 닫기
        closesocket(connectSocket);
        //winsock dll 사용 종료
        WSACleanup();

        //프로그램 종료
        return 1;

    }

    printf("Connect to Server\n");

    while (true)
    {
        //TODO
        Sleep(1000);
    }

  
    closesocket(connectSocket);

    WSACleanup();
}
