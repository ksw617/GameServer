#include "pch.h"
#include "SocketHelper.h"

int main()
{
	SocketHelper::Init();

	SOCKET socket = SocketHelper::CreateSocket();

	if (socket == INVALID_SOCKET)
	{
		printf("QQQQQ\n");
	}

	SocketHelper::BindAny(socket, 27015);

	SocketHelper::Listen(socket);

	SOCKET acceptSocket = accept(socket, NULL, NULL);

	cout << "Client Connected!" << endl;

	SocketHelper::Clear();

	while (true)
	{

	}
	return 0;
}