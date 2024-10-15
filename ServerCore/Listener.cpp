#include "pch.h"
#include "Listener.h"
#include "ServerService.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"
#include "IocpEvent.h"


Listener::~Listener()
{
    CloseSocket();
}

bool Listener::StartAccept(ServerService* service)
{
    //ó�� �����ɶ� �ް�
    serverService = service;

    socket = SocketHelper::CreateSocket();
    if (socket == INVALID_SOCKET)
        return false;

    if (!SocketHelper::SetReuseAddress(socket, true))
        return false;


    if (!SocketHelper::SetLinger(socket, 0, 0))
        return false;
 
    ULONG_PTR key = 0;
    service->GetIocpCore()->Register(this);

    if (!SocketHelper::Bind(socket, service->GetSockAddr()))
        return false;

    if (!SocketHelper::Listen(socket))
        return false;


    AcceptEvent* acceptEvent = new AcceptEvent;
    acceptEvent->owner = this;
    RegisterAccept(acceptEvent);

    return true;
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    //�̰� ��ſ�
    //Session* session = new Session;
    Session* session = serverService->CreateSession();
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
    Session* session = acceptEvent->session;

    if (!SocketHelper::SetUpdateAcceptSocket(session->GetSocket(), socket))
    {
        printf("SetUpdateAcceptSocket Error\n");
        RegisterAccept(acceptEvent);
        return;
    }

    printf("ProcessAccept\n");

    SOCKADDR_IN sockAddr;
    int sockAddrSize = sizeof(sockAddr); 
    //Accept Socket�� �־��ָ� sockAddr���ٰ� Ŭ���� �ּ� ������ �־���
    //SOCKET_ERROR��� ���� �ִ°Ŵϱ� ���� ó��
    if (getpeername(session->GetSocket(), (SOCKADDR*)&sockAddr, &sockAddrSize) == SOCKET_ERROR)
    {
        //����ó��
        printf("getpeername Error\n");
        RegisterAccept(acceptEvent);
        return;
    }

    //session�� Ŭ���� �ּҸ� ������Ʈ
    session->SetSockAddr(sockAddr);

    //����ȰŴϱ� �ش� Session�� ProcessConnect�� ���������
    session->ProcessConnect();

    //���� �մ� ���� �غ�. �ٸ� Ŭ�� �����Ҽ� �ְ� Accept���ó��
    RegisterAccept(acceptEvent);   
}

void Listener::CloseSocket()
{
    SocketHelper::CloseSocket(socket);
}



void Listener::ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred)
{
    ProcessAccept((AcceptEvent*)iocpEvent);
}
