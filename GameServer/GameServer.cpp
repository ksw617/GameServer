#include <iostream>
#include <thread>  
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 


struct Session
{
    WSAOVERLAPPED overlapped = {};
    //����
    SOCKET socket = INVALID_SOCKET;
    //���� ����
    char recvBuffer[512] = {};
};

void RecvThread(HANDLE iocpHandle)
{
    DWORD byteTransferred = 0;
    ULONG_PTR key = 0;
    //WSAOVERLAPPED overlapped = {};
    Session* session = nullptr;


    while (true)
    {
        printf("Waiting....\n");


        GetQueuedCompletionStatus(iocpHandle, &byteTransferred, &key, (LPOVERLAPPED*)&session, INFINITE);

        printf("Recv : %s\n", session->recvBuffer);

        //printf("recv Length : %d\n", byteTransferred); 
        //printf("recv key : %p\n", key);        

        //���� ���� �� ��Ÿ ������ �����Ͽ� �ٽ� ������ ���� �غ�
        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;                    //���Ź��� ����
        wsaBuf.len = sizeof(session->recvBuffer);            //������ ũ�� ����

        DWORD recvLen = 0;                                   //���ŵ� ������ ���̸� ������ ����
        DWORD flags = 0;                                     //flags ���� ������� ����
       
        //�񵿱� ������ �ٽ� ����. ���������� ������ ������ ���� �ݺ�
        WSARecv(session->socket, OUT & wsaBuf, 1, OUT & recvLen, &flags, &session->overlapped, NULL);
    }
}

int main()
{
    printf("============== Server  ================\n");
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2,2);
            
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStarup filed with error\n");
        return 1;
    }


    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {                  
        printf("socket function failed with error : %d\n", WSAGetLastError());
    }
    else  
    {
        printf("socket function succeded\n");
    }

    SOCKADDR_IN service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(27015);

    if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("bind failed with error : %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;

    }
          
    if (listen(listenSocket, 10) == SOCKET_ERROR)
    {
        printf("listen failed with error : %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
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
            printf("accept failed with error : %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }

        printf("Client Connected\n");
        ULONG_PTR key = 0;
        CreateIoCompletionPort((HANDLE)acceptSocket, iocpHandle, key, 0);

        Session* session = new Session;
        //���� ���
        session->socket = acceptSocket;

        WSABUF wsaBuf;
        //buffer -> session->recvBuffer
        wsaBuf.buf = session->recvBuffer; 
        wsaBuf.len = sizeof(session->recvBuffer);

        DWORD recvLen = 0;
        DWORD flags = 0;
        //WSAOVERLAPPED overlapped = {};

        WSARecv(acceptSocket, OUT &wsaBuf, 1, OUT &recvLen, &flags,  &session->overlapped, NULL);

    }
  

    t.join();

    closesocket(listenSocket);
    WSACleanup();
    
    return 0;
}
