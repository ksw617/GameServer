#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int32 bufferSize)
{
	//버퍼크기 할당
	buffer.resize(bufferSize);
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::CopyData(void* data, int32 len)
{

	//충분한 여유공간이 있다면 써야할 데이터 보다
	CONDITION_CRASH(Capacity() >= len);
	//data(시작점) len(길이)만큼 복사해서 넣어줌 
	memcpy(buffer.data(), data, len);
	//쓴 사이즈는 len
	writeSize = len;


}
