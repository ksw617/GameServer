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

SOCKET Session::GetSocket()
{
	return socket;
}

void Session::SetNetworkAddress(NetworkAddress address)
{
	networkAddress = address;
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
