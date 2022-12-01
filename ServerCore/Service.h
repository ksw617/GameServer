#pragma once
#include <functional>
#include "NetworkAddress.h"


enum class SERVICE_TYPE : uint8
{
	SERVER,
	CLIENT,
};

class IocpCore;
class Session;

using SessionFactory = function<shared_ptr<Session>(void)>;

class Service : public enable_shared_from_this<Service>
{
protected:
	mutex lock;

	SERVICE_TYPE type;
	NetworkAddress address = {};
	shared_ptr<IocpCore> iocpCore;
protected:
	SessionFactory sessionFactory;
	set<shared_ptr<Session>> sessions;
	int32 sessionCount = 0;
public:
	Service(SERVICE_TYPE _type, NetworkAddress _address, shared_ptr<IocpCore> _core, SessionFactory _factory);
	virtual ~Service();
public:
	virtual bool Start() abstract;
	virtual void EndService();
public:
	SERVICE_TYPE GetType() { return type; }
	NetworkAddress GetNetworkAddress() { return address; }
	shared_ptr<IocpCore>& GetIocpCore() { return iocpCore; }
	int32 GetSessionCount() { return sessionCount; }
	
public:
	void SetSessionFactory(SessionFactory func) { sessionFactory = func; }
public:
	shared_ptr<Session> CreateSession();
	void AddSession(shared_ptr<Session> session);
	void RemoveSession(shared_ptr<Session> session);

};

