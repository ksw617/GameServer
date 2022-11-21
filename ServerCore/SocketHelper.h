#pragma once
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
	static bool BindAny(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void Close(SOCKET& socket);

};

