#include "pch.h"
#include "SocketHelper.h"

void SocketHelper::Init()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSAData wsaData;

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup ¿¡·¯\n");
	}
}
