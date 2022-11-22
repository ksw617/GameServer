#pragma once
#include "IocpCore.h"
#include "NetworkAddress.h"

class Session : public IocpObject
{
public:
	char recvBuffer[512];
private:
	SOCKET socket = INVALID_SOCKET;
	NetworkAddress networkAddress = {};
	atomic<bool> connected = false;
public:
	virtual HANDLE GetHandle() override;
	virtual void Observe(class IocpEvent* iocpEvent, int32 bytes = 0) override;
public:
	SOCKET GetSocket();
	void SetNetworkAddress(NetworkAddress address);
	NetworkAddress GetNetworkAddress();
public:
	Session();
	~Session();
};

