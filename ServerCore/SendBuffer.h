#pragma once
class SendBuffer : enable_shared_from_this<SendBuffer>
{
private:
	//버퍼 
	vector<BYTE> buffer;
	//사용한 크기
	int32 writeSize = 0;
public:
	//버퍼 크기 할당
	SendBuffer(int32 bufferSize);
	~SendBuffer();
public:
	//버퍼 데이터 반환
	BYTE* Buffer() { return buffer.data(); }
	//사용한 크기 반환
	int32 WriteSize() { return writeSize; }
	//버퍼의 크기 반환
	int32 Capacity() { return static_cast<int32>(buffer.size()); }
public:
	//데이터 복사
	void CopyData(void* data, int32 len);

};

