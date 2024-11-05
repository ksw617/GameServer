#pragma once
#include "Task.h"

class TaskQueue : public enable_shared_from_this<TaskQueue>
{
protected:
	shared_mutex rwLock;
	atomic<int> taskCount = 0;
	queue<shared_ptr<Task>> tasks;
public:
	void CallAsync(Callback&& callback)
	{
		Push(make_shared<Task>(move(callback)));
	}

	template<typename ReturnType, typename ClassType, typename... Args>
	void CallAsync(ReturnType(ClassType::* memFunc)(Args...), Args... args)
	{
		shared_ptr<ClassType> pointer = static_pointer_cast<ClassType>(shared_from_this());
		Push(make_shared<Task>(pointer, memFunc, std::forward<Args>(args)...));
	}

private:
	void Push(shared_ptr<Task>&& task);
public:
	void Execute();
	void Clear();
};

