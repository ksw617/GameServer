#include "pch.h"
#include "SendBufferManager.h"

//static thread_local ���� �ʱ�ȭ
thread_local shared_ptr<SendBufferChunk> SendBufferManager::localSendBufferChunk = nullptr;

shared_ptr<SendBuffer> SendBufferManager::Open(int size)
{
    //���� Open�� ȣ�� ���� ���
    if (localSendBufferChunk == nullptr)
    {
        localSendBufferChunk = Pop(); // ������ �ƴ� ����
        localSendBufferChunk->Init(); // pop�� SendBufferChunk �ʱ�ȭ

    }

    //localSendBufferChunk�� ���� �ִ� ���� == ����ϰ� �ִ� ����
    if (localSendBufferChunk->IsOpen())
        return nullptr;

    //Chunk �������� ���� ������ ���� ��� �ϴ� ũ�⺸�� �۴ٸ�
    if (localSendBufferChunk->FreeSize() < size)
    {
        //���Ӱ� pop�ؼ� ����
        //���� �����״� Chunk�� -> PushGlobal �Լ� ȣ�� �Ǹ鼭
        //PushGlobal(SendBufferChunk* bufferChunck) <- ���� ���ڷ� ��
        localSendBufferChunk = Pop();
        localSendBufferChunk->Init(); // pop�� SendBufferChunk �ʱ�ȭ

    }


    //localSendBufferChunk�� size��ŭ ����Ҽ� �ִ� sendBuffer�� ��ȯ
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

