#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <thread> // 스레드 사용

struct Session
{
    WSAOVERLAPPED overlapped = {};  
    SOCKET socket = INVALID_SOCKET; 
    char sendBuffer[512] = {};      
};

void SendThread(HANDLE iocpHandle)
{
    DWORD byteTransferred = 0;
    ULONG_PTR key = 0;
    Session* session = nullptr;

    while (true)
    {
        printf("Waiting...\n");
        if (GetQueuedCompletionStatus(iocpHandle, &byteTransferred, &key, (LPOVERLAPPED*)&session, INFINITE))
        {
            WSABUF wsaBuf;
            wsaBuf.buf = session->sendBuffer;           //송신 버퍼 지정
            wsaBuf.len = sizeof(session->sendBuffer);   //버퍼의 크기 지정


            DWORD sendLen = 0;  // 송신 데이터 길이를 저장할 변수
            DWORD flags = 0;    // flag, 현재는 사용하지 않음

            printf("Send : %s\n", session->sendBuffer);
                
            //비동기로 Send
            if (WSASend(session->socket, &wsaBuf, 1, &sendLen, flags, &session->overlapped, NULL))
            {
                //Send 실패시 오류 메세지 출력
                printf("Send failed with error %d\n", WSAGetLastError());
                //while문 종료
                break;
            }
        }
       

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

    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connectSocket == INVALID_SOCKET)
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

    if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("connect function failed with error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;

    }

    printf("Connect to Server\n");

    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
    thread t(SendThread, iocpHandle);

    ULONG_PTR key = 0;

    CreateIoCompletionPort((HANDLE)connectSocket, iocpHandle, key, 0);

    Session* session = new Session;
    session->socket = connectSocket;
    char sendBuffer[512] = "Hello this is Client";

    memcpy(session->sendBuffer, sendBuffer, sizeof(sendBuffer));


    WSABUF wsaBuf;
    wsaBuf.buf = session->sendBuffer;
    wsaBuf.len = sizeof(session->sendBuffer);


    DWORD sendLen = 0;
    DWORD flags = 0;

    WSASend(connectSocket, &wsaBuf, 1, &sendLen, flags, &session->overlapped, NULL);

    t.join();

    closesocket(connectSocket);

    WSACleanup();

    return 0;
}
