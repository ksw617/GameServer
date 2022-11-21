#include "pch.h"
#include "SocketHelper.h"

LPFN_ACCEPTEX SocketHelper::lpfnAcceptEx = NULL;

void SocketHelper::Init()
{
    WORD wVersionRequested = MAKEWORD(2,2);
    WSAData wsaData;
    CONDITION_CRASH(WSAStartup(wVersionRequested, &wsaData) == 0);

    SOCKET sock = CreateSocket();
    CONDITION_CRASH(SocketMode(sock, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&lpfnAcceptEx)));
}

void SocketHelper::Clear()
{
    WSACleanup();
}

SOCKET SocketHelper::CreateSocket()
{
    return WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketHelper::SocketMode(SOCKET socket, GUID guid, LPVOID* lpfn)
{
    DWORD bytes = 0;
    bool result = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), lpfn, sizeof(lpfn), &bytes, NULL, NULL);
    return result != SOCKET_ERROR;
}

bool SocketHelper::BindAny(SOCKET socket, uint16 port)
{
    SOCKADDR_IN service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(port);

    bind(socket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service));

    return true;

}

bool SocketHelper::Listen(SOCKET socket, int32 backlog)
{
    return listen(socket, backlog) != SOCKET_ERROR;
}

void SocketHelper::Close(SOCKET& socket)
{
    if (socket != INVALID_SOCKET)
    {
        closesocket(socket);
        socket = INVALID_SOCKET;
    }
}

