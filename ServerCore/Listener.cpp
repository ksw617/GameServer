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
    //Todo

}

HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(socket);
}

void Listener::Observe(IocpEvent* iocpEvent, int32 bytes)
{   
    CONDITION_CRASH(iocpEvent->GetType() != IO_TYPE::ACCEPT);
    AcceptEvent* acceptEvnet = reinterpret_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvnet);
    
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

    if (SocketHelper::SetLinger(socket, 0,0) == false)
    {
        return false;
    }

    if (SocketHelper::BindAny(socket, address.GetPort()) == false)
    {
        return false;
    }
    if (SocketHelper::Listen(socket, 10) == false)
    {
        return false;
    }

    AcceptEvent* acceptEvent = new AcceptEvent;
    acceptEvents.push_back(acceptEvent);
    RegisterAccept(acceptEvent);

    return true;
}

void Listener::CloseSocket()
{
}

Listener::~Listener()
{
}
