#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 _bufferSize)
{
    bufferSize = _bufferSize;

    //버퍼크기를 늘림
    capacity = bufferSize * BufferCount;
    //버퍼의 크기는 capacity 만큼
    buffer.resize(capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clean()
{
    int32 dataSize = DataSize();
    //위치가 같은 확률이 높아짐
    if (dataSize == 0)
    {
        readPos = 0;
        writePos = 0;
    }
    else
    {  
        //여유공간 보다 써야될 공간이 적을 경우에만
        if (FreeSize() < bufferSize)
        {
            //메모리 복사가 이루어짐
            memcpy(&buffer[0], &buffer[readPos], dataSize);
            readPos = 0;
            writePos = dataSize;
        }
    }

}

bool RecvBuffer::OnRead(int32 bytes)
{
    if (bytes > DataSize())
    {
        return false;
    }

    readPos += bytes;
    return true;
}

bool RecvBuffer::OnWrite(int32 bytes)
{
    if (bytes > FreeSize())
    {
        return false;
    }

    writePos += bytes;
    return true;
}
