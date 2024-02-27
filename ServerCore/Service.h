#pragma once
#include "CorePch.h"
class Service
{
private:
	SOCKADDR_IN sockAddr = {};
public:
	Service(wstring ip, u_short port);
	~Service();
};

