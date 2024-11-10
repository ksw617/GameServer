#pragma once
#include <functional>

using Callback = function<void()>;

class Task
{
private:
	Callback callback;
public:
	Task(Callback&& func) : callback(std::move(func)) {}

	template<typename ReturnType, typename ClassType, typename... Args>
	Task(shared_ptr<ClassType> pointer, ReturnType(ClassType::* memFunc)(Args...), Args&&... args)
	{
		callback = [pointer, memFunc, args...]()
			{
				(pointer.get()->*memFunc)(args...);
			};
	}

	void Execute()
	{
		callback();
	}
};

