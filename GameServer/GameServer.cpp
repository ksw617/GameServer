#pragma once
#include "pch.h"
#include <ServerService.h>
#include <IocpCore.h>


int main()
{
    printf("============== Server  ================\n");

    Service* serverService = new ServerService(L"127.0.0.1", 27015);

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

    //서비스 실행
    serverService->Start();

    t.join();

    //해제
    delete serverService;


    return 0;
}
