#pragma once
#include "IocpObj.h"

class AcceptEvent;
class ServerService;

class Listener : public IocpObj
{
private:
	//ServerSercie Ãß°¡
	ServerService* serverService = nullptr;
	SOCKET socket = INVALID_SOCKET;
public:
	Listener() = default;
	virtual ~Listener();
public:
	bool StartAccept(ServerService* service);
	void RegisterAccept(AcceptEvent* acceptEvent);	
	void ProcessAccept(AcceptEvent* acceptEvent);
	void CloseSocket();
	

public:
	HANDLE GetHandle() override { return (HANDLE)socket; };
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;
};

