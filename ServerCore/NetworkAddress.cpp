#include "pch.h"
#include "NetworkAddress.h"

NetworkAddress::NetworkAddress(SOCKADDR_IN sockAddrIn)
{
	service	= sockAddrIn;

}

NetworkAddress::NetworkAddress(wstring ip, uint16 port)
{
	//메모리 밀어버림
	memset(&service, 0, sizeof(service));
	//ipv4 or ipv6
	service.sin_family = AF_INET;
	//ip정함
	IN_ADDR address;
	InetPtonW(AF_INET, ip.c_str(), &address);
	service.sin_addr = address;
	//포트정함
	service.sin_port = htons(port);
}

SOCKADDR_IN& NetworkAddress::GetSockAddrIn()
{
	return service;
}

wstring NetworkAddress::GetIp()
{
	WCHAR buffer[100];
	InetNtopW(AF_INET, &service.sin_addr, buffer, sizeof(buffer) / sizeof(WCHAR));
	return wstring(buffer);
}

uint16 NetworkAddress::GetPort()
{
	return ntohs(service.sin_port);
}
