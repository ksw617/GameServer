#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(socket);
}

void Session::Observe(IocpEvent* iocpEvent, int32 bytes)
{
    //Todo
}

void Session::SetNetAddress(NetworkAddress address)
{
    networkAddress = address;
}

SOCKET Session::GetSocket()
{
    return socket;
}

NetworkAddress Session::GetNetworkAddress()
{
    return networkAddress;
}

Session::Session()
{
    socket = SocketHelper::CreateSocket();
}

Session::~Session()
{
    SocketHelper::Close(socket);
}
