#include "pch.h"
#include "SocketHelper.h"
#include "Listener.h"

int main()
{
	SocketHelper::Init();

	Listener listener;
	NetworkAddress address(L"127.0.0.1", 27015);
	CONDITION_CRASH(listener.StartAccept(address));

	while (true)
	{
		GIocpCore.Observe();

	}

	return 0;
}