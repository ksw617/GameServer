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

    //���� �����
    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket function failed with error : %d\n", WSAGetLastError());
    }
    else
    {
        printf("socket function succeded\n");
    }

    //������ �ּ�
    SOCKADDR_IN service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(27015);

	//TODO

    //������ ���� service(������ ���� ����)
    //connect(�� ����, ������ �ּ�, �ش� �ּ��� ����ü ũ��)
    if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        //���� �ڵ� Ȯ��
        printf("connect function failed with error : %d\n", WSAGetLastError());
        //���� �ݱ�
        closesocket(connectSocket);
        //winsock dll ��� ����
        WSACleanup();

        //���α׷� ����
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
