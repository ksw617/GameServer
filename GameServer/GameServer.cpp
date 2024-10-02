#include <iostream>
#include <thread>  
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 
#include <MSWSock.h> // 호출


void AcceptThread(HANDLE iocpHandle)
{

    DWORD bytesTransferred = 0;
    ULONG_PTR key = 0;
    WSAOVERLAPPED overlapped = {};

    while (true)
    {
        printf("Waiting....\n");

        if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
        {
            printf("Client Connected\n");
        }
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

#pragma region Accpet 관리해줄 일꾼 만듬.
    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
    ULONG_PTR key = 0;
    CreateIoCompletionPort((HANDLE)listenSocket, iocpHandle, key, 0);
    thread t(AcceptThread, iocpHandle);
#pragma endregion

#pragma region 비동기 Accept 함수 만듬
    DWORD dwBytes;
    LPFN_ACCEPTEX lpfnAcceptEx = NULL;
    GUID guidAcceptEx = WSAID_ACCEPTEX;
    if (WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx),
        &lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
    {
        printf("WSAIoctl ConnectEx filed with error : %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;

    }
#pragma endregion

#pragma region 빈 Accept용 소켓 만듬
    SOCKET acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (acceptSocket == INVALID_SOCKET)
    {
        printf("Accept socket function failed with error : %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
#pragma endregion


#pragma region 비동기 Accept 함수 호출

    char acceptBuffer[1024];
    WSAOVERLAPPED overlapped = {};

    if (lpfnAcceptEx(listenSocket, acceptSocket, acceptBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,&dwBytes, &overlapped) == FALSE)
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            printf("AcceptEx filed with error : %d\n", WSAGetLastError());
            closesocket(listenSocket);
            closesocket(acceptSocket);
            WSACleanup();
            return 1;
        }

    }
#pragma endregion


    t.join();

    closesocket(listenSocket);
    WSACleanup();
    
    return 0;
}
