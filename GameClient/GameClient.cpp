#include "pch.h"

#include <SocketHelper.h>
#include <ThreadManager.h>
#include <ClientService.h>
#include <Session.h>
#include <SendBuffer.h>

char sendData[] = "Hello world";

class GameSession : public Session
{
public:
	~GameSession()
	{
		printf("소멸\n");
	}
public:
	virtual void OnConnected() override
	{
		printf("Connected to Server\n");

		//sendBuffer 할당
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);
		//sendBuffer에 있는 buffer에 sendData에 복사
		sendBuffer->CopyData(sendData, sizeof(sendData));
		//보내기
		Send(sendBuffer);
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		printf("OnRecv Length : %d bytes\n", len);

		this_thread::sleep_for(1s);

		//sendBuffer 할당
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(4096);
		//sendBuffer에 있는 buffer에 sendData에 복사
		sendBuffer->CopyData(sendData, sizeof(sendData));
		//보내기
		Send(sendBuffer);

		return len;
	}

	virtual void OnSend(int32 len) override
	{
		printf("OnSend Length : %d bytes\n", len);
	}

	virtual void OnDisconnected() override
	{
		printf("Disconnected\n");
	}

};

int main()
{
	ThreadManager* threadManager = new ThreadManager();
	SocketHelper::Init();

	this_thread::sleep_for(1s);

	shared_ptr<ClientService> service = make_shared<ClientService>
		(
			NetworkAddress(L"127.0.0.1", 27015),
			make_shared<IocpCore>(),
			[]() {return make_shared<GameSession>(); }
	);

	CONDITION_CRASH(service->Start());

	threadManager->Call([=]()
		{
			while (true)
			{
				service->GetIocpCore()->Observe();
			}
		});

	threadManager->Join();

	delete threadManager;
	return 0;

}