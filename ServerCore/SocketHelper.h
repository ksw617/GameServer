#pragma once
class SocketHelper
{
public:
	static void Init();
	static void Clear();

public:
	static SOCKET CreateSocket();
	static bool SocketMode(SOCKET socket, GUID guid, LPVOID* lpfn);
};

