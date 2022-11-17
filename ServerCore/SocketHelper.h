#pragma once
#include "NetworkAddress.h"
class SocketHelper
{

public:
	static LPFN_ACCEPTEX lpfnAcceptEx;

public:
	static void Init();
	static void Clear();

public:
	static SOCKET CreateSocket();
	static bool SocketMode(SOCKET socket, GUID guid, LPVOID* lpfn);
public:
	static bool Bind(SOCKET socket, NetworkAddress address);
	static bool BindAny(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void Close(SOCKET& socket);
public:
	static bool SetKeepAlive(SOCKET socket, bool enable);
	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 time);
	static bool SetReuseAddress(SOCKET socket, bool enable);
	static bool SetTcpNoDelay(SOCKET socket, bool enable);
};
