#pragma once
#include "pch.h"
#include <ServerService.h>
#include <Session.h>

class ServerSession : public Session
{
public:
    ~ServerSession()
    {
        printf("~ServerSession");
    }
public:
    virtual void OnConnected() override
    {
        printf("Connected...\n");
    }

    virtual int OnRecv(BYTE* buffer, int len) override
    {
        printf("Recv : %s\n", buffer);

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
        printf("OnSend Len : %d\n", len);
    }

    virtual void OnDisconnected() override
    {
        printf("Disconnected...\n");
    }
};

int main()
{
    printf("============== Server  ================\n");

    shared_ptr<Service> serverService = make_shared<ServerService>(L"127.0.0.1", 27015, []() {return make_shared<ServerSession>(); });

    thread t
    (
        [=]() 
        {
            while (true)
            {
                serverService->ObserveIO();
            }
        }
    );

    serverService->Start();

    t.join();

    return 0;
}
