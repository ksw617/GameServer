#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"

void ThreadManager::InitTLS()
{
	static atomic<uint32> SThreadID = 1;
	LThreadID = SThreadID.fetch_add(1);
}

void ThreadManager::Join()
{
	for (thread& t : threads)// threads를 돌면서
	{
		if (t.joinable()) // 기다려야 한다는 거면
		{
			t.join();// 해당 스레드가 끝날때 까지 기다림
		}
	}

	threads.clear(); // 해당 vector를 비움
}

void ThreadManager::Call(function<void()> callback)
{
	//스레드 + 매개변수로 받은 callback 함수를 연결
	lock_guard<mutex> lockGuard(lock); // lock을 잡고

	//threads에 람다로 함수{TLS를 초기화 하는 함수 호출 & callback 연결되어 있는 함수} 만들어서 추가.
	threads.push_back(thread([=]() {

		InitTLS();
		callback();

		}));
}

ThreadManager::ThreadManager()
{
	//TLS 초기화 호출
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	//ThreadManager 해제 될때 해당 스레드 다 끝날때까지 기다림.
	Join();
}
