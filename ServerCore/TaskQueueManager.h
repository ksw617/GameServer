#pragma once
class TaskQueue;
class TaskQueueManager
{
private:
	TaskQueueManager() {}
	~TaskQueueManager() {}
public:
	static TaskQueueManager& Get()
	{
		static TaskQueueManager instance;
		return instance;
	}
public:
	TaskQueueManager(const TaskQueueManager&) = delete;
	TaskQueueManager& operator=(const TaskQueueManager&) = delete;

private:
	shared_mutex rwLock;
	queue<shared_ptr<TaskQueue>> taskQueues;
public:
	thread_local static TaskQueue* localTaskQueue;
	thread_local static ULONGLONG workTime;
public:
	void Push(shared_ptr<TaskQueue> taskQueue);
	shared_ptr<TaskQueue> Pop();

	// 남아있는 모든 TaskQueue를 처리하는 함수
	void ProcessRemainingTasks();
};
