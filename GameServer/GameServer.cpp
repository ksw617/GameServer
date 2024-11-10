#pragma once
#include "pch.h"
#include <ServerService.h>
#include <TaskQueueManager.h>
#include <IocpCore.h>

#include "ClientSession.h"
#include "ClientPacketHandler.h"



#define THREAD_COUNT 5

enum
{
	WORKER_TICK = 64
};

static void ExecuteTask(shared_ptr<ServerService>& service)
{
	while (true)
	{
		TaskQueueManager::Get().workTime = GetTickCount64() + WORKER_TICK;

		service->GetIocpCore()->ObserveIO(10);

		//抗距等 老皑 贸府
		TaskQueueManager::Get().DistributeReservedTasks();

		TaskQueueManager::Get().ProcessRemainedTasks();
	}
}

int main()
{
	printf("============= Server =============\n");

	ClientPacketHandler::Init();

	shared_ptr<ServerService> service = make_shared<ServerService>(L"127.0.0.1", 27015, []() {return make_shared<ClientSession>(); });

	if (!service->Start())
	{
		printf("Server Start Error\n");
		return 1;
	}

	vector<thread> threads;

	for (int i = 0; i < THREAD_COUNT; i++)
	{
		threads.push_back(thread
		([&service]()
			{
				ExecuteTask(service);
			}
		));
	}

	ExecuteTask(service);


	for (int i = 0; i < THREAD_COUNT; i++)
	{
		if (threads[i].joinable())
		{
			threads[i].join();
		}
	}


	return 0;
}
