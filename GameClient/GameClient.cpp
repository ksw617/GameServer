#pragma once
#include "pch.h"
#include <ClientService.h>
#include <Session.h>


char sendBuffer[] = "Hello this is Client";

class ClientSession : public Session
{
public:
    virtual void OnConnected() override
    {
        printf("Connect to Server\n");

        Send((BYTE*)sendBuffer, sizeof(sendBuffer));
    }

    virtual int OnRecv(BYTE* buffer, int len) override
    {
        printf("Recv : %s\n", (char*)buffer);

        this_thread::sleep_for(1s);

        Send(buffer, len);
        return len;
    }

    virtual void OnSend(int len) override
    {

    }

    virtual void OnDisconnected() override
    {
    }
};

int main()
{
    this_thread::sleep_for(1s);

    printf("============== Client  ================\n");

    Service* clientService = new ClientService(L"127.0.0.1", 27015, []() {return new ClientSession; });

    if (!clientService->Start())
    {
        printf("Can not Start\n");
        return 1;

    }

    thread t
    (
        [=]()
        {
            while (true)
            {
                clientService->ObserveIO();
            }
        }
    );


    t.join();

    delete clientService;


    return 0;
}
