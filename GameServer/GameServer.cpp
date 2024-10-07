#pragma once
#include "pch.h"
#include <Service.h>
#include <Listener.h>

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

    Service service(L"127.0.0.1", 27015);

    Listener listener;

    thread t(AcceptThread, listener.GetHandle());

    listener.StartAccept(service);

    t.join();

    return 0;
}
