#pragma once
#include "pch.h"
#include <ClientService.h>
#include <Session.h>


char sendData[] = "Hello this is Client";

class ClientSession : public Session
{
public:
    ~ClientSession()
    {
        printf("~ClientSession");
    }
public:
    virtual void OnConnected() override
    {
        printf("Connect to Server\n");

        //sendBuffer 생성하면서 4096할당
        shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);


        //데이터 복사 하고
        if (sendBuffer->CopyData(sendData, sizeof(sendData)))
        {
            //문제없음 데이터 보내기
            Send(sendBuffer);
        }

    }

    virtual int OnRecv(BYTE* buffer, int len) override
    {
        printf("Recv : %s\n", (char*)buffer);

        this_thread::sleep_for(1s);

        //sendBuffer 생성하면서 4096할당
        shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);

        //데이터 복사 하고
        if (sendBuffer->CopyData(buffer, len))
        {
            //문제없음 데이터 보내기
            Send(sendBuffer);
        }

        return len;
    }

    virtual void OnSend(int len) override
    {

    }

    virtual void OnDisconnected() override
    {
        printf("Disconnected...\n");
    }
};

int main()
{
    this_thread::sleep_for(1s);

    printf("============== Client  ================\n");
     shared_ptr<Service> clientService = make_shared<ClientService>(L"127.0.0.1", 27015, []() {return make_shared<ClientSession>(); });

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

    return 0;
}
