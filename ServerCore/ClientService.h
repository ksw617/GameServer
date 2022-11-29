#pragma once
#include "Service.h"
class ClientService	: public Service
{
public:
	ClientService(NetworkAddress _address, shared_ptr<IocpCore> _core, SessionFactory _factory)
		:Service::Service(SERVICE_TYPE::Server, _address, _core, _factory) {}
	~ClientService() {}
};

