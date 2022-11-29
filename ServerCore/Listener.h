#pragma once
#include "IocpCore.h"
#include "NetworkAddress.h"

class AcceptEvent;
class ServerService;

class Listener : public IocpObject
{
protected:
	SOCKET socket = INVALID_SOCKET;
	vector<AcceptEvent*> acceptEvents;
	shared_ptr<ServerService> service;
private:
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvnet);
public:
	virtual HANDLE GetHandle() override;
	virtual void Observe(class IocpEvent* iocpEvent, int32 bytes = 0) override;
public:
	bool StartAccept(shared_ptr<ServerService> _service);
	void CloseSocket();
public:
	Listener() = default;
	~Listener();
};

