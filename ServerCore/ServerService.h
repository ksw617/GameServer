#pragma once
#include "Service.h"
class ServerService	: public Service
{
private:
	shared_ptr<class Listener> listener = nullptr;

public:
	ServerService(NetworkAddress _address, shared_ptr<IocpCore> _core, SessionFactory _factory)
		:Service::Service(SERVICE_TYPE::Server, _address, _core, _factory) { }
	~ServerService() {}
public:
	virtual bool Start() override;
	virtual void EndService();

};

