#pragma once
//TaskQueue Queue
class TaskQueue;
class TaskQueueManager
{
private:
	TaskQueueManager() = default;
	~TaskQueueManager() = default;
public:
	static TaskQueueManager& Get()
	{
		static TaskQueueManager instance;
		return instance;
	}
public:
	TaskQueueManager(const TaskQueueManager&) = delete;
	TaskQueueManager& operator= (const TaskQueueManager&) = delete;
private:
	shared_mutex rwLock;
	queue<shared_ptr<TaskQueue>> taskQueues;
public:
	thread_local static TaskQueue* localTaskQueue;
	thread_local static ULONGLONG workTime;
public:
	void Push(shared_ptr<TaskQueue> taskQueue);
	shared_ptr<TaskQueue> Pop();
	void ProcessRemainedTasks();
	//예약 시스템 추가
	void DistributeReservedTasks();
};

