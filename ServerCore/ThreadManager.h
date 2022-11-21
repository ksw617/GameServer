#pragma once
class ThreadManager
{
private:
	mutex lock;
	vector<thread> threads;
public:
	static void InitTLS();
	void Join();
	void Call(function<void()> callback);

public:
	ThreadManager();
	~ThreadManager();
};

