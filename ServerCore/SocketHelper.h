#pragma once
#include "NetworkAddress.h"
class SocketHelper
{
public:
	//Rename
	static LPFN_CONNECTEX ConnectEx;
	//DisConnect Ãß°¡
	static LPFN_DISCONNECTEX DisconnectEx;
	//Rename
	static LPFN_ACCEPTEX AcceptEx;
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
	static bool SetKeppAlive(SOCKET socket, bool enable);
	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 time);
	static bool SetReuseAddress(SOCKET socket, bool enable);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);
	static bool SetTcpNoDelay(SOCKET socket, bool enable);


};

