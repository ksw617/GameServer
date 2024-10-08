#pragma once
#include "pch.h"
#include <Service.h>
#include <Listener.h>
#include <IocpCore.h>

void AcceptThread(HANDLE iocpHandle)
{

    DWORD bytesTransferred = 0;
    ULONG_PTR key = 0;
    WSAOVERLAPPED overlapped = {};

    while (true)
    {
        printf("Waiting....\n");

        if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
        {
            printf("Client Connected\n");
        }
    }
}

int main()
{
    printf("============== Server  ================\n");

    Service* serverService = new Service(L"127.0.0.1", 27015);
    thread t(AcceptThread, serverService->GetIocpCore()->GetHandle());

    //서비스 실행
    serverService->Start();

    t.join();

    //해제
    delete serverService;


    return 0;
}
