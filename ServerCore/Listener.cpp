#include "pch.h"
#include "Listener.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"
#include "IocpEvent.h"


Listener::~Listener()
{
    CloseSocket();
}

bool Listener::StartAccept(Service* service)
{
    socket = SocketHelper::CreateSocket();
    if (socket == INVALID_SOCKET)
        return false;

    if (!SocketHelper::SetReuseAddress(socket, true))
        return false;


    if (!SocketHelper::SetLinger(socket, 0, 0))
        return false;
 
    ULONG_PTR key = 0;
    //내꺼 넣어주고
    service->GetIocpCore()->Register(this);

    if (!SocketHelper::Bind(socket, service->GetSockAddr()))
        return false;

    if (!SocketHelper::Listen(socket))
        return false;

    AcceptEvent* acceptEvent = new AcceptEvent;
    //AcceptEvent는 Listener랑 연결
    acceptEvent->owner = this;
    RegisterAccept(acceptEvent);

    return true;
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    Session* session = new Session;
    acceptEvent->Init();
    acceptEvent->session = session;

    DWORD dwBytes = 0;
               
    if (!SocketHelper::AcceptEx(socket, session->GetSocket(), session->recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, (LPOVERLAPPED)acceptEvent))
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
            RegisterAccept(acceptEvent);
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    printf("ProcessAccept\n");
    //Todo
}

void Listener::CloseSocket()
{
    SocketHelper::CloseSocket(socket);
}


//GetQueuedCompletionStatus 호출되면 할당된애가 Listener라면 여리고 들어옴
void Listener::ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred)
{
    ProcessAccept((AcceptEvent*)iocpEvent);
}
