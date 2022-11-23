#include "pch.h"
#include "Listener.h"
#include "SocketHelper.h"
#include "IocpEvent.h"
#include "Session.h"

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    Session* session = new Session;
    acceptEvent->Init();
    acceptEvent->SetSession(session);

    DWORD dwBytes = 0;
    if (!SocketHelper::lpfnAcceptEx(socket, session->GetSocket(), session->recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, reinterpret_cast<LPOVERLAPPED>(acceptEvent)))
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            RegisterAccept(acceptEvent);
        }
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvnet)
{
    Session* session = acceptEvnet->GetSession();
    if (!SocketHelper::SetUpdateAcceptSocket(session->GetSocket(), socket))
    {
        RegisterAccept(acceptEvnet);
        return;
    }

    SOCKADDR_IN sockAddress;
    int32 size = sizeof(sockAddress);
    if (getpeername(session->GetSocket(), reinterpret_cast<SOCKADDR*>(&sockAddress), &size) == SOCKET_ERROR)
    {
        RegisterAccept(acceptEvnet);
        return;
    }
    session->SetNetworkAddress(NetworkAddress(sockAddress));

    printf("Client Connected\n");

    RegisterAccept(acceptEvnet);

}

HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(socket);
}

void Listener::Observe(IocpEvent* iocpEvent, int32 bytes)
{   
    CONDITION_CRASH(iocpEvent->GetType() == IO_TYPE::ACCEPT);
    AcceptEvent* acceptEvnet = reinterpret_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvnet);
    
}

bool Listener::StartAccept(NetworkAddress address)
{
    socket = SocketHelper::CreateSocket();
    
    if (socket == INVALID_SOCKET)
    {
        printf("socket error");
        return false;
    }

    if (GIocpCore.Register(this) == false)
    {
        printf("Register error");
        return false;
    }

    if (SocketHelper::SetReuseAddress(socket, true) == false)
    {
        printf("SetReuseAddress error");
        return false;
    }

    if (SocketHelper::SetLinger(socket, 0,0) == false)
    {
        printf("SetLinger error");
        return false;
    }

    if (SocketHelper::Bind(socket, address) == false)
    {
        printf("BindAny error");
        return false;
    }
    if (SocketHelper::Listen(socket) == false)
    {
        printf("Listen error");
        return false;
    }

    AcceptEvent* acceptEvent = new AcceptEvent;
    acceptEvents.push_back(acceptEvent);
    RegisterAccept(acceptEvent);

    return true;
}

void Listener::CloseSocket()
{
    SocketHelper::Close(socket);
}

Listener::~Listener()
{
    SocketHelper::Close(socket);
    for (AcceptEvent* acceptEvent : acceptEvents)
    {
        delete acceptEvent;
    }

    acceptEvents.clear();
}
