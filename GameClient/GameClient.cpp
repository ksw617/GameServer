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

        //sendBuffer �����ϸ鼭 4096�Ҵ�
        shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);


        //������ ���� �ϰ�
        if (sendBuffer->CopyData(sendData, sizeof(sendData)))
        {
            //�������� ������ ������
            Send(sendBuffer);
        }

    }

    virtual int OnRecv(BYTE* buffer, int len) override
    {
        printf("Recv : %s\n", (char*)buffer);

        this_thread::sleep_for(1s);

        //sendBuffer �����ϸ鼭 4096�Ҵ�
        shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);

        //������ ���� �ϰ�
        if (sendBuffer->CopyData(buffer, len))
        {
            //�������� ������ ������
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
