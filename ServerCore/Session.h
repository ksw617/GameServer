#pragma once
#include "IocpCore.h"
#include "NetworkAddress.h"

class Session : public IocpObj
{
public:
	char recvBuffer[512];
private:
	SOCKET socket = INVALID_SOCKET;
	NetworkAddress networkAddress = {};
	atomic<bool> connected = false;								  
public:
	Session();
	~Session();
public:
	virtual HANDLE GetHandle() override;
	virtual void Observe(class IocpEvent* iocpEvent, int32 bytes = 0) override;
public:
	void SetNetWorkAddress(NetworkAddress address);
	NetworkAddress GetNetworkAddress();
	SOCKET GetSocket();

};

