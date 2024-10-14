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
	
	//Accept ������ �뵵�� 
	void ProcessAccept(AcceptEvent* acceptEvent);
	void CloseSocket();
	

public:
	//�����Լ� ������
	HANDLE GetHandle() override { return (HANDLE)socket; };
	void ObserveIO(IocpEvent* iocpEvent, DWORD byteTransferred) override;
};

