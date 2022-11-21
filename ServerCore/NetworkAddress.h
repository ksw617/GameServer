#pragma once
class NetworkAddress
{
public:
	SOCKADDR_IN service = {};
public:
	NetworkAddress() = default;
	NetworkAddress(SOCKADDR_IN sockAddrIn);
	NetworkAddress(wstring ip, uint16 port);
public:
	SOCKADDR_IN& GetSockAddrIn();
	wstring GetIp();
	uint16 GetPort();

};

