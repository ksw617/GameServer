#include "pch.h"

#include "SocketHelper.h"
#include "Listener.h"


//27015
int main()
{
	Listener listener;
	NetworkAddress address(L"127.0.0.1", 27015);
	listener.StartAccept(address);

	while (true)
	{
		GIocpCore.Observe();

	}


	return 0;
}