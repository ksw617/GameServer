#include "pch.h"
#include "TaskTimer.h"
#include "TaskQueue.h"


void TaskTimer::Reserve(UINT64 tickAfter, weak_ptr<TaskQueue> owner, shared_ptr<Task> task)
{
    const UINT64 executeTick = GetTickCount64() + tickAfter; // 실행 시간 계산
    TaskData* taskData = new TaskData(owner, task); // 새로운 TaskData 인스턴스 생성

    unique_lock<shared_mutex> lock(rwLock); // 쓰기 잠금
    items.push(TimerItem{ executeTick, taskData }); // 우선순위 큐에 타이머 아이템 추가
}

void TaskTimer::Distribute(UINT64 now)
{
    if (distributing.exchange(true) == true) // 이미 Distribute 작업이 진행 중인지 확인
        return;

    vector<TimerItem> timerItems; // 실행할 타이머 아이템들을 임시 저장할 벡터

    {
        unique_lock<shared_mutex> lock(rwLock); // 쓰기 잠금

        while (items.empty() == false) // 우선순위 큐가 비어있지 않은 동안
        {
            const TimerItem& timerItem = items.top(); // 가장 우선순위가 높은 아이템 확인
            if (now < timerItem.executeTick) // 현재 시각이 실행 시각보다 이르면 중단
                break;

            timerItems.push_back(timerItem); // 실행할 아이템을 벡터에 추가
            items.pop(); // 큐에서 아이템 제거
        }
    }

    for (TimerItem& item : timerItems) // 모든 실행할 아이템에 대해
    {
        if (shared_ptr<TaskQueue> owner = item.taskData->owner.lock()) // TaskQueue가 여전히 존재하는지 확인
            owner->Push(item.taskData->task); // TaskQueue에 Task 추가

        delete item.taskData; // TaskData 인스턴스 해제

    }

    distributing.store(false); // Distribute 작업 완료
}

void TaskTimer::Clear()
{
    unique_lock<shared_mutex> lock(rwLock); // 쓰기 잠금
    while (items.empty() == false) // 우선순위 큐가 비어있지 않은 동안
    {
        const TimerItem& timerItem = items.top(); // 가장 우선순위가 높은 아이템 확인
        delete timerItem.taskData; // TaskData 인스턴스 해제
        items.pop(); // 큐에서 아이템 제거
    }
}