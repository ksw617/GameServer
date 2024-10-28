#pragma once
#include "pch.h"
#include <ServerService.h>
#include <PacketSession.h>       //PacketSession 사용
#include <SendBufferManager.h>

class ServerSession : public PacketSession
{
public:
    ~ServerSession()
    {
        printf("~ServerSession");
    }
public:
    virtual void OnConnected() override
    {
    }

    //OnRecv가 아니라 OnRecvPacket
    virtual int OnRecvPacket(BYTE* buffer, int len) override
    {

        shared_ptr<SendBuffer> sendBuffer = SendBufferManager::Get().Open(4096);
        
        memcpy(sendBuffer->GetBuffer(), buffer, len);
       
        if (sendBuffer->Close(len))
        {
            Send(sendBuffer);
        }
        
        printf("%s\n", &buffer[4]);

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
    printf("============== Server  ================\n");

    shared_ptr<Service> serverService = make_shared<ServerService>(L"127.0.0.1", 27015, []() {return make_shared<ServerSession>(); });


    //serverService 시작하기
    if (!serverService->Start())
    {
        printf("Server Service Error\n");
        return 1;

    }

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

    t.join();

    return 0;
}
