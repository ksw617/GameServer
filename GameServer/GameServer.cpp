#include <iostream>
#include <thread>  
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

void RecvThread(HANDLE iocpHandle)
{
    DWORD byteTransferred = 0;
    ULONG_PTR key = 0;
    WSAOVERLAPPED overlapped = {};

    while (true)
    {
        printf("Waiting....\n");


        GetQueuedCompletionStatus(iocpHandle, &byteTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE);

        printf("recv Length : %d\n", byteTransferred); 
        printf("recv key : %p\n", key);        

        //걸어줘야 하는데 acceptSocket을 넣어줄수 없어 <- Send한 client랑 소통할 소켓
        //WSARecv(acceptSocket, OUT & wsaBuf, 1, OUT & recvLen, &flags, &overlapped, NULL);
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

    //수신 데이터 버퍼
    //= {}; 0으로 초기화
    char recvBuffer[512] = {};

    while (true)
    {
 
        SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
        printf("Hello\n");
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


        WSABUF wsaBuf;
        wsaBuf.buf = recvBuffer; 
        wsaBuf.len = sizeof(recvBuffer);

        DWORD recvLen = 0;
        DWORD flags = 0;
        WSAOVERLAPPED overlapped = {};

        WSARecv(acceptSocket, OUT &wsaBuf, 1, OUT &recvLen, &flags,  &overlapped, NULL);

    }
  

    t.join();

    closesocket(listenSocket);
    WSACleanup();
    
    return 0;
}
