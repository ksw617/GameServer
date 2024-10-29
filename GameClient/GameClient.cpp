#pragma once
#include "pch.h"
#include <ClientService.h>

#include "ServerSession.h"


int main()
{
    this_thread::sleep_for(1s);

    printf("============== Client  ================\n");
     shared_ptr<Service> clientService = make_shared<ClientService>(L"127.0.0.1", 27015, []() {return make_shared<ServerSession>(); });

     //1000명 정도 접속 시작
     for (int i = 0; i < 1000; i++)
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
