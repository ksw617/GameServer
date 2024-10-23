#include "pch.h"
#include "SendBufferManager.h"

//static thread_local 변수 초기화
thread_local shared_ptr<SendBufferChunk> SendBufferManager::localSendBufferChunk = nullptr;

shared_ptr<SendBuffer> SendBufferManager::Open(int size)
{
    //최초 Open을 호출 했을 경우
    if (localSendBufferChunk == nullptr)
    {
        localSendBufferChunk = Pop(); // 기존꺼 아님 새거
        localSendBufferChunk->Init(); // pop된 SendBufferChunk 초기화

    }

    //localSendBufferChunk가 열려 있는 상태 == 사용하고 있는 상태
    if (localSendBufferChunk->IsOpen())
        return nullptr;

    //Chunk 데이터의 여유 공간이 내가 써야 하는 크기보다 작다면
    if (localSendBufferChunk->FreeSize() < size)
    {
        //새롭게 pop해서 연결
        //기존 가르켰던 Chunk는 -> PushGlobal 함수 호출 되면서
        //PushGlobal(SendBufferChunk* bufferChunck) <- 여기 인자로 들어감
        localSendBufferChunk = Pop();
        localSendBufferChunk->Init(); // pop된 SendBufferChunk 초기화

    }


    //localSendBufferChunk에 size만큼 사용할수 있는 sendBuffer를 만환
    return localSendBufferChunk->Open(size);

}

shared_ptr<SendBufferChunk> SendBufferManager::Pop()
{
    {
        unique_lock<shared_mutex> lock(rwLock);

        if (!sendBufferChunks.empty())
        {
            shared_ptr<SendBufferChunk> sendBufferChunk = sendBufferChunks.back();
            sendBufferChunks.pop_back();

            return sendBufferChunk;

        }
    }

    return  shared_ptr<SendBufferChunk>(new SendBufferChunk, PushGlobal);
}


void SendBufferManager::PushGlobal(SendBufferChunk* bufferChunck)
{
    Get().Push(shared_ptr<SendBufferChunk>(bufferChunck, PushGlobal));
  
}

void SendBufferManager::Push(shared_ptr<SendBufferChunk> bufferChunk)
{
    unique_lock<shared_mutex> lock(rwLock);
    sendBufferChunks.push_back(bufferChunk);
}

