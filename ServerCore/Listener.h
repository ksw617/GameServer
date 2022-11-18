#pragma once
#include "IocpCore.h"
#include "NetworkAddress.h"

class AcceptEvent;

class Listener : public IocpObj
{
protected:
	SOCKET socket = INVALID_SOCKET;
	vector<AcceptEvent*> acceptEvents;
public:
	virtual HANDLE GetHandle() override;
	virtual void Observe(class IocpEvent* iocpEvent, int32 bytes = 0) override;
public:
	bool StartAccept(NetworkAddress address);
	void CloseSocket();
private:
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
public:
	Listener() = default;
	~Listener();

};

