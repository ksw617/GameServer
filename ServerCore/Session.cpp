#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"

Session::Session()
{
	socket = SocketHelper::CreateSocket();	
}

Session::~Session()
{
	SocketHelper::CloseSocket(socket);
}

void Session::ProcessConnect()
{
	printf("ProcessConnect\n");
}

void Session::ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred)
{
}
