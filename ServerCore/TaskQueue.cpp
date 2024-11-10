#include "pch.h"
#include "TaskQueue.h"	
#include "TaskQueueManager.h"

void TaskQueue::Push(shared_ptr<Task> task, bool  pushOnly)
{
	const int prevCount = taskCount.fetch_add(1);

	{
		unique_lock<shared_mutex> lock(rwLock);
		tasks.push(task);
	}

	if (prevCount == 0)
	{
		if (TaskQueueManager::Get().localTaskQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			TaskQueueManager::Get().Push(shared_from_this());
		}

	}
}

void TaskQueue::Execute()
{
	TaskQueueManager::Get().localTaskQueue = this;

	while (true)
	{
		vector<shared_ptr<Task>> works;

		{
			unique_lock<shared_mutex> lock(rwLock);

			while (!tasks.empty())
			{
				works.push_back(tasks.front());
				tasks.pop();
			}
		}

		const int workSize = works.size();
		for (int i = 0; i < workSize; i++)
			works[i]->Execute();

		//남은 일이 0개라면
		if (workSize == taskCount.fetch_sub(workSize))
		{
			TaskQueueManager::Get().localTaskQueue = nullptr;
			return;
		}

		const ULONGLONG now = GetTickCount64();
		if (now >= TaskQueueManager::Get().workTime)
		{
			TaskQueueManager::Get().localTaskQueue = nullptr;
			TaskQueueManager::Get().Push(shared_from_this());
			return;
		}
	}
}

void TaskQueue::Clear()
{
	unique_lock<shared_mutex> lock(rwLock);
	tasks = queue<shared_ptr<Task>>();
}
