#include "pch.h"
#include "SocketHelper.h"
#include "ServerService.h"
#include "Session.h"
#include "ThreadManager.h"
#include <SendBuffer.h>

class GameSession : public Session
{
public:
	~GameSession()
	{
		printf("소멸\n");
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		printf("Recv Data Length : %d byte\n", len);
		printf("Recv Data : %s\n", (char*)buffer);
		
		//sendBuffer 할당
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);
		//sendBuffer에 있는 buffer에 받은 버퍼 복사
		sendBuffer->CopyData(buffer, sizeof(len));
		//보내기
		Send(sendBuffer);

		return len;
	}

	virtual void OnSend(int32 len) override
	{
		printf("Send Data Length : %d byte\n", len);
	}					 

};

int main()
{
	ThreadManager* threadManager = new ThreadManager();
	SocketHelper::Init();

	shared_ptr<ServerService> service = make_shared<ServerService>
		(
		NetworkAddress(L"127.0.0.1", 27015),
		make_shared<IocpCore>(),
		[]() {return make_shared<GameSession>(); });
	
	CONDITION_CRASH(service->Start());

	threadManager->Call([=]() 
		{
			while (true)
			{
				service->GetIocpCore()->Observe();

			}
		});

	threadManager->Join();

	delete(threadManager);


	return 0;
}