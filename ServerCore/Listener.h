#pragma once
#include "IocpObj.h"

class AcceptEvent;

class Listener : public IocpObj
{
private:
	SOCKET socket = INVALID_SOCKET;
public:
	Listener() = default;
	~Listener();
public:
	bool StartAccept(class Service* service);
	void RegisterAccept(AcceptEvent* acceptEvent);
	
	//Accept 진행할 용도로 
	void ProcessAccept(AcceptEvent* acceptEvent);
	void CloseSocket();
	

public:
	//가상함수 제정의
	HANDLE GetHandle() override { return (HANDLE)socket; };
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;
};

