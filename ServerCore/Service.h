#pragma once
class IocpCore;
class Session;

//Session* (�Ű����� ����) { return session; }
using SessionFactory = function<Session*(void)>;

enum class ServiceType : uint8
{
	NONE,
	SERVER, 
	CLIENT,
};

class Service
{
private:
	ServiceType serviceType = ServiceType::NONE;
	SOCKADDR_IN sockAddr = {};
	IocpCore* iocpCore = nullptr;
protected:
	shared_mutex rwLock;
	//Session�� ������
	set<Session*> sessions;
	//Session ���� ������ �ϳ��� ���� ������ ���� ����
	int sessionCount = 0;
	//��ϵ� Session����� �ִ� factory
	SessionFactory sessionFactory;
public:
	//Service�� �����ɶ� SessionFactory�� �Ҵ�
	Service(ServiceType type, wstring ip, uint16 port, SessionFactory factory);
	virtual ~Service();
public:	//Get�Լ�
	ServiceType GetServiceType() const { return serviceType; }
	SOCKADDR_IN& GetSockAddr() { return sockAddr; }
	IocpCore* GetIocpCore() const { return iocpCore; }
	int GetSessionCount() const { return sessionCount; }
public:
	//set<Session*> sessions;
	Session* CreateSession();
	void AddSession(Session* session);
	void RemoveSession(Session* session);
	
public:
	virtual bool Start() abstract;
	bool ObserveIO(DWORD time = INFINITE);
};

