#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"

Session::Session()
{
    socket = SocketHelper::CreateSocket();
}

Session::~Session()
{
    SocketHelper::Close(socket);
}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(socket);
}

void Session::Observe(IocpEvent* iocpEvent, int32 bytes)
{
    //TODO
}

void Session::SetNetWorkAddress(NetworkAddress address)
{
    networkAddress = address;
}

NetworkAddress Session::GetNetworkAddress()
{
    return networkAddress;
}

SOCKET Session::GetSocket()
{
    return socket;
}
