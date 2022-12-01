#include "pch.h"
#include "Service.h"
#include "IocpCore.h"
#include "Session.h"

Service::Service(SERVICE_TYPE _type, NetworkAddress _address, shared_ptr<IocpCore> _core, SessionFactory _factory)
{
	type = _type;
	address = _address;
	iocpCore = _core;
	sessionFactory = _factory;
}

Service::~Service()
{
}

void Service::EndService()
{

}

shared_ptr<Session> Service::CreateSession()
{
	shared_ptr<Session> session = sessionFactory();	//return shared_ptr<Session>;
	//Ãß°¡
	session->SetService(shared_from_this());
	if (iocpCore->Register(session) == false)
	{
		return nullptr;
	}
	return session;
}

void Service::AddSession(shared_ptr<Session> session)
{

	lock_guard<mutex> guard(lock);
	sessionCount++;
	sessions.insert(session);
}

void Service::RemoveSession(shared_ptr<Session> session)
{
	lock_guard<mutex> guard(lock);
	sessionCount--;
	sessions.erase(session);
}
