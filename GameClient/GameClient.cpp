#pragma once
#include "pch.h"
#include <ClientService.h>
#include <PacketSession.h>     //PacketSession
#include <SendBufferManager.h>


char sendData[] = "Hello this is Client";

class ClientSession : public PacketSession
{
public:
    ~ClientSession()
    {
        printf("~ClientSession");
    }
public:
    virtual void OnConnected() override
    {

        shared_ptr<SendBuffer> sendBuffer = SendBufferManager::Get().Open(4096);

        //주소 받아오기
        BYTE* data = sendBuffer->GetBuffer();

        //     25    =             4        +    21
        int sendSize = sizeof(PacketHeader) + sizeof(sendData);

        //[2byte : size] = 25
        ((PacketHeader*)data)->size = sendSize;
        //[size][2byte : id] = 0
        ((PacketHeader*)data)->id = 0;

        //[size][id][....여기 채우면 됨]   <- 21바이트 복사해서 채우기
        memcpy(&data[4], sendData, sizeof(sendData));

        //토탈 25바이트 사용한거니까
        if (sendBuffer->Close(sendSize))
        {
            Send(sendBuffer);
        }

    }

    virtual int OnRecvPacket(BYTE* buffer, int len) override
    {

        this_thread::sleep_for(1s);
        shared_ptr<SendBuffer> sendBuffer = SendBufferManager::Get().Open(4096);

        memcpy(sendBuffer->GetBuffer(), buffer, len);

        if (sendBuffer->Close(len))
        {
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

     //1000명 정도 접속 시작
     for (int i = 0; i < 1; i++)
     {
         if (!clientService->Start())
         {
             printf("Can not Start\n");
             return 1;

         }
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
