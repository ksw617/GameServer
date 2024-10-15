#pragma once
class IocpCore;
class Session;

//Session* (매개변수 없고) { return session; }
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
	//Session들 담을꺼
	set<Session*> sessions;
	//Session 담을 때마다 하나씩 증가 빠질때 마다 감소
	int sessionCount = 0;
	//등록된 Session만들어 주는 factory
	SessionFactory sessionFactory;
public:
	//Service가 생성될때 SessionFactory에 할당
	Service(ServiceType type, wstring ip, uint16 port, SessionFactory factory);
	virtual ~Service();
public:	//Get함수
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

