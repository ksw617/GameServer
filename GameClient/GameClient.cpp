#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 
#include <MSWSock.h> 

#include <thread> 

void ConnectThread(HANDLE iocpHandle)
{
    DWORD bytesTransferred = 0;
    ULONG_PTR key = 0;
    WSAOVERLAPPED overlapped = {};

    while (true)
    {
        printf("Waiting....\n");

        if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
        {
            printf("Successed\n");
        }
    }
    
}

int main()
{               
    printf("============== Client  ================\n");

#pragma region Win Socket ����

    //�������� 1�� �ʰ� ���α׷� ����
    this_thread::sleep_for(1s);

    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 2);

    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStarup filed with error\n");
        return 1;
    }
#pragma endregion


#pragma region ���� ����
    SOCKET connectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("WSASocket filed with error : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
#pragma endregion

#pragma region Ŭ���̾�Ʈ �ּ� == ���� �ּ�

    SOCKADDR_IN clientService;
    memset(&clientService, 0, sizeof(clientService));
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = htonl(INADDR_ANY);
    clientService.sin_port = htons(0);

#pragma endregion

#pragma region �� ������ �� �ּ� ����
    if (bind(connectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
    {
        printf("bind filed with error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }
#pragma endregion


#pragma region Connect �������� �ϲ� ����.
    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
    ULONG_PTR key = 0;
    CreateIoCompletionPort((HANDLE)connectSocket, iocpHandle, key, 0);
    thread t(ConnectThread, iocpHandle);
#pragma endregion


    DWORD dwBytes;

#pragma region �񵿱� Connect �Լ� ����

 
    LPFN_CONNECTEX lpfnConnectEx = NULL; 
    GUID guidConnectEx = WSAID_CONNECTEX;
    if (WSAIoctl(connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx),
        &lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
    {
        printf("WSAIoctl ConnectEx filed with error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;

    }
#pragma endregion

#pragma region �񵿱� Disconnect �Լ� ����


    //Disconnect �Լ� ������
    LPFN_DISCONNECTEX lpfnDisconnectEx = NULL;
    //GUID �� Disconnect ������ 
    GUID guidDisconnectEx = WSAID_DISCONNECTEX;
    if (WSAIoctl(connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidDisconnectEx, sizeof(guidDisconnectEx),
        &lpfnDisconnectEx, sizeof(lpfnDisconnectEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
    {
        printf("WSAIoctl DisconnectEx filed with error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;

    }
#pragma endregion

#pragma region ������ ���� �ּ�
    SOCKADDR_IN serverService;
    memset(&serverService, 0, sizeof(serverService));
    serverService.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serverService.sin_addr);
    serverService.sin_port = htons(27015);
#pragma endregion

    DWORD bytesTransferred = 0;
    WSAOVERLAPPED overlapped = {};

#pragma region �񵿱� Connect �Լ� �� == ��������

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
#pragma endregion



#pragma region �񵿱� Disconnect �Լ� �� == ���� ������

    //�Լ������͸� ���ؼ� �Լ� ��
    if (lpfnDisconnectEx(connectSocket, &overlapped, 0, 0) == FALSE)
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            printf("DisconnectEx filed with error : %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }

    }
#pragma endregion

    
    t.join();

    closesocket(connectSocket);
    WSACleanup();

    return 0;
}
