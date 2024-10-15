#include "pch.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"

Service::Service(ServiceType type, wstring ip, uint16 port, SessionFactory factory) : serviceType(type), sessionFactory(factory)
{
    if (!SocketHelper::StartUp())
        return;

    memset(&sockAddr, 0, sizeof(sockaddr));
    sockAddr.sin_family = AF_INET;

    IN_ADDR address{};
    InetPton(AF_INET, ip.c_str(), &address);
    sockAddr.sin_addr = address;
    sockAddr.sin_port = htons(port);

    iocpCore = new IocpCore;
}

Service::~Service()
{
    if (iocpCore != nullptr)
    {
        delete iocpCore;
        iocpCore = nullptr;
    }


    SocketHelper::CleanUp();
}


Session* Service::CreateSession()
{
    //session 만들어 주고
    Session* session = sessionFactory();

    //false라면 nullptr
    if (!iocpCore->Register(session))
        return nullptr;

    return session;
   
}

void Service::AddSession(Session* session)
{
    unique_lock<shared_mutex> lock(rwLock);
    sessionCount++;
    sessions.insert(session);
}

void Service::RemoveSession(Session* session)
{
    unique_lock<shared_mutex> lock(rwLock);
    sessions.erase(session);
    sessionCount--;
}

bool Service::ObserveIO(DWORD time)
{
    if (iocpCore != nullptr)
    {
        return iocpCore->ObserveIO(time);
    }

    return false;
}
