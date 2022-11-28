#pragma once
#include "NetworkAddress.h"

enum class Type : uint8
{
	Server,
	Client,
};

class Service
{
public:
	Type type;
	NetworkAddress networkAddr = {};
	shared_ptr<class IocpCore> iocpCore;


};

class ClientService : public Service
{};

class ServerService : public Service
{};