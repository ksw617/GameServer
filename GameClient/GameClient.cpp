#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 
#include <MSWSock.h> // 추가

#include <thread> 

//비동기 연결 완료를 대기하고 결과를 처리하는 스레드 함수
void ConnectThread(HANDLE iocpHandle)
{
    DWORD bytesTransferred = 0;
    ULONG_PTR key = 0;
    WSAOVERLAPPED overlapped = {};

    printf("Waiting....\n");

    //대기
    if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE)) 
    {
        printf("Connect successed\n"); // 연결 성공 메세지 출력
    }

    //안꺼지게 1초에 한번씩 돌림
    while (true)
    {
        this_thread::sleep_for(1s);
    }
    
}

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

    SOCKET connectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("WSASocket filed with error : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    DWORD dwBytes;
    LPFN_CONNECTEX lpfnConnectEx = NULL;
    GUID guidConnectEx = WSAID_CONNECTEX;
    if (WSAIoctl(connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx),
       &lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
    {
        printf("WSAIoctl filed with error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;

    }

    SOCKADDR_IN serverService;
    memset(&serverService, 0, sizeof(serverService));
    serverService.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serverService.sin_addr);
    serverService.sin_port = htons(27015);

    SOCKADDR_IN clientService;
    memset(&clientService, 0, sizeof(clientService));
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = htonl(INADDR_ANY); 
    clientService.sin_port = htons(0);


    if (bind(connectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
    {
        printf("bind filed with error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
    ULONG_PTR key = 0;
    CreateIoCompletionPort((HANDLE)connectSocket, iocpHandle, key, 0);

    thread t(ConnectThread, iocpHandle);

    DWORD bytesTransferred = 0;
    WSAOVERLAPPED overlapped = {};

    if (lpfnConnectEx(connectSocket, (SOCKADDR*)&serverService, sizeof(serverService), nullptr, 0, &bytesTransferred, &overlapped) == FALSE)
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            printf("ConnectEx filed with error : %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
           
    }

    t.join();

    closesocket(connectSocket);
    WSACleanup();

    return 0;
}
