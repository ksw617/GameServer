#include "pch.h"
#include "TaskQueueManager.h"
#include "TaskQueue.h"

//static thread_local 변수 초기화
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
	{
		return nullptr;
	}

	shared_ptr<TaskQueue> taskQueue = taskQueues.front();
	taskQueues.pop();
	return taskQueue;

}

void TaskQueueManager::ProcessRemainingTasks()
{
	while (true)
	{
		ULONGLONG now = GetTickCount64();
		if (now > workTime)
			break;

		shared_ptr<TaskQueue> taskQueue = Pop();
		if (taskQueue == nullptr)
			break;

		taskQueue->Execute();
	}
}
