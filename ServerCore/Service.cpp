#include "pch.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Listener.h"

Service::Service(wstring ip, uint16 port)
{
    SocketHelper::StartUp();

    sockAddr.sin_family = AF_INET;
    IN_ADDR address;
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

    if (listener != nullptr)
    {
        delete listener;
        listener = nullptr;
    }

    SocketHelper::CleanUp();
}

bool Service::Start()
{
    listener = new Listener;
    return listener->StartAccept(this);
}

bool Service::ObserveIO(DWORD time)
{
    if (iocpCore != nullptr)
    {
        return iocpCore->ObserveIO(time);
    }

    return false;
}
