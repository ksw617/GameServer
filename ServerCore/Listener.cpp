#include "pch.h"
#include "Listener.h"
#include "SocketHelper.h"
#include "IocpEvent.h"
#include "Session.h"


HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(socket);
}

void Listener::Observe(IocpEvent* iocpEvent, int32 bytes)
{
    CONDITION_CRASH(iocpEvent->GetType() == IO_TYPE::ACCEPT);
    AcceptEvent* acceptEvent = reinterpret_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvent);
}

bool Listener::StartAccept(NetworkAddress address)
{
    socket = SocketHelper::CreateSocket();
    if (socket == INVALID_SOCKET)
    {
        return false;
    }

    if (GIocpCore.Register(this) == false)
    {
        return false;
    }

    if (SocketHelper::SetReuseAddress(socket, true) == false)
    {
        return false;
    }

    if (SocketHelper::SetLinger(socket, 0, 0) == false)
    {
        return false;
    }

    if (SocketHelper::Bind(socket, address) == false)
    {
        return false;
    }

    if (SocketHelper::Listen(socket) == false)
    {
        return false;
    }

    AcceptEvent* acceptEvent = new AcceptEvent;
    acceptEvents.push_back(acceptEvent);
    RegisterAccept(acceptEvent);


    return false;
}

void Listener::CloseSocket()
{
    SocketHelper::Close(socket);
}

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

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    Session* session = acceptEvent->GetSession();
    if (!SocketHelper::SetUpdateAcceptSocket(session->GetSocket(), socket))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    SOCKADDR_IN sockAddress;
    int32 size = sizeof(sockAddress);
    if (getpeername(session->GetSocket(), reinterpret_cast<SOCKADDR*>(&sockAddress), &size) == SOCKET_ERROR)
    {
        RegisterAccept(acceptEvent);
        return;
    }

    session->SetNetAddress(NetworkAddress(sockAddress));

    printf("Client Connected");

    RegisterAccept(acceptEvent);
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
