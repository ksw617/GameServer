#pragma once
#include "pch.h"
#include <ServerService.h>

#include "ClientSession.h"
#include "SessionManager.h"

#include <SendBufferManager.h>

#include "Protocol.pb.h" // include

#define THREAD_COUNT 5

int main()
{
    printf("============== Server  ================\n");

    shared_ptr<Service> serverService = make_shared<ServerService>(L"127.0.0.1", 27015, []() {return make_shared<ClientSession>(); });


    //serverService 시작하기
    if (!serverService->Start())
    {
        printf("Server Service Error\n");
        return 1;

    }

    vector<thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads.push_back(thread
        (
            [=]()
            {
                while (true)
                {
                    serverService->ObserveIO();
                }
            }
        ));
    }
   

    while (true)
    {
        
        Protocol::TEST packet;
        packet.set_id(1);
        packet.set_hp(2);
        packet.set_mp(3);

        uint16 dataSize = (uint16)packet.ByteSizeLong();
        uint16 packetSize = dataSize + sizeof(PacketHeader);   //dataSize + 4byte



        shared_ptr<SendBuffer> sendBuffer = SendBufferManager::Get().Open(4096);
        BYTE* buffer = sendBuffer->GetBuffer();

        ((PacketHeader*)buffer)->size = packetSize;
        ((PacketHeader*)buffer)->id = 0;

        //복사
        packet.SerializePartialToArray(&buffer[4], dataSize);

     
        if (sendBuffer->Close(packetSize))
        {

            SessionManager::Get().Broadcast(sendBuffer);

        }

        this_thread::sleep_for(250ms);

    }


    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (threads[i].joinable())
        {
            threads[i].join();
        }
    }
 

    return 0;
}
