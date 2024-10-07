#pragma once
#include "CorePch.h"

class Service
{
private:
	SOCKADDR_IN sockAddr = {};
public:
	SOCKADDR_IN GetSockAddr() const { return sockAddr; }
public :
	Service(wstring ip, uint16 port);
	~Service();
};

