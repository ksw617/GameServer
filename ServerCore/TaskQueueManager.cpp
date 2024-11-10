#include "pch.h"
#include "TaskQueueManager.h"
#include "TaskQueue.h"
#include "TaskTimer.h"

thread_local TaskQueue* TaskQueueManager::localTaskQueue = nullptr;
thread_local ULONGLONG TaskQueueManager::workTime = 0;

void TaskQueueManager::Push(shared_ptr<TaskQueue> taskQueue)
{
	unique_lock<shared_mutex> lock(rwLock);
	taskQueues.push(taskQueue);
}

shared_ptr<TaskQueue> TaskQueueManager::Pop()
{
	unique_lock<shared_mutex> lock(rwLock);
	if (taskQueues.empty())
		return nullptr;

	shared_ptr<TaskQueue> taskQueue = taskQueues.front();
	taskQueues.pop();

	return taskQueue;
}

void TaskQueueManager::ProcessRemainedTasks()
{
	while (true)
	{
		ULONGLONG now = GetTickCount64();
		if (now > workTime)
		{
			break;
		}

		shared_ptr<TaskQueue> taskQueue = Pop();
		if (taskQueue == nullptr)
			break;

		taskQueue->Execute();

	}
}

void TaskQueueManager::DistributeReservedTasks()
{
	const UINT64 now = GetTickCount64();

	TaskTimer::Get().Distribute(now);

}
