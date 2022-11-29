#include "pch.h"
#include "SocketHelper.h"
#include "Listener.h"

#include "ServerService.h"
#include "Session.h"

int main()
{
	SocketHelper::Init();

	shared_ptr<ServerService> service = make_shared<ServerService>
		(
		NetworkAddress(L"127.0.0.1", 27015),
		make_shared<IocpCore>(),
		[]() {return make_shared<Session>(); });
	
	CONDITION_CRASH(service->Start());

	while (true)
	{
		service->GetIocpCore()->Observe();

	}

	return 0;
}