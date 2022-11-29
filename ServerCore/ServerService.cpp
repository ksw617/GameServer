#include "pch.h"
#include "ServerService.h"	 
#include "Listener.h"

bool ServerService::Start()
{
	if (sessionFactory == nullptr)
	{
		return false;
	}

	listener = make_shared<Listener>();
	if (listener == nullptr)
	{
		return false;
	}
	shared_ptr<ServerService> service = static_pointer_cast<ServerService>(shared_from_this());
	if (listener->StartAccept(service) == false)
	{

		return false;
	}

    return true;
}

void ServerService::EndService()
{
	Service::EndService();
}
