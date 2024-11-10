#pragma once

class TaskQueue;
class Task;

struct TaskData
{
    // TaskData 구조체의 생성자: TaskQueue에 대한 weak_ptr과 Task에 대한 shared_ptr를 받아 멤버 변수를 초기화합니다.
    TaskData(weak_ptr<TaskQueue> owner, shared_ptr<Task> task) : owner(owner), task(task) {    }

    weak_ptr<TaskQueue> owner; // TaskQueue의 약한 참조를 유지합니다. 실제 객체가 사라져도 안전하게 참조 가능.
    shared_ptr<Task> task; // Task에 대한 공유 포인터. Task의 생명 주기를 관리합니다.
};

struct TimerItem
{
    // TimerItem들을 비교하기 위한 연산자 오버로딩. 실행 시간(executeTick)을 기준으로 비교합니다.
    bool operator < (const TimerItem& other) const
    {
        return executeTick > other.executeTick;
    }

    UINT64 executeTick = 0; // 이 타이머 아이템이 실행될 시각 (시스템 틱 단위)
    TaskData* taskData = nullptr; // 실행할 TaskData의 포인터

};

class TaskTimer
{
private:
    TaskTimer() = default;
    ~TaskTimer() = default;
public:
    static TaskTimer& Get()
    {
        static TaskTimer instance;
        return instance;
    }
public:
    TaskTimer(const TaskTimer&) = delete;
    TaskTimer& operator= (const TaskTimer&) = delete;
private:
    shared_mutex rwLock; // 읽기/쓰기 잠금을 위한 shared_mutex
    priority_queue<TimerItem> items; // TimerItem들을 우선순위 큐로 관리합니다.
    atomic<bool> distributing = false; // 현재 Distribute 작업이 진행 중인지 나타내는 플래그
public:
    void Reserve(UINT64 tickAfter, weak_ptr<TaskQueue> owner, shared_ptr<Task> task); // 지정된 시간 후에 Task를 실행하기 위해 예약합니다.
    void Distribute(UINT64 now); // 예약된 Task들 중 현재 시간(now)에 실행해야 할 Task들을 배포합니다.
    void Clear(); // 모든 예약된 Task들을 취소하고 관련 리소스를 정리합니다.

};