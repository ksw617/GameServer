#include "pch.h"
#include "Service.h"

Service::Service(wstring ip, uint16 port)
{

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStarup filed with error\n");
        return;
    }

    sockAddr.sin_family = AF_INET;
    IN_ADDR address;
    InetPton(AF_INET, ip.c_str(), &address);
    sockAddr.sin_addr = address;
    sockAddr.sin_port = htons(port);
}

Service::~Service()
{
    WSACleanup();
}
