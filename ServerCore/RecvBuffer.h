#pragma once

class RecvBuffer
{
private:
	//10배
	const int32 BufferCount = 10;
	int32 bufferSize = 0;
	//총 크기(BufferCount * bufferSize)
	int32 capacity = 0;
	int32 readPos = 0;
	int32 writePos = 0;
	vector<BYTE> buffer; 
public:
	RecvBuffer(int32 _bufferSize);
	~RecvBuffer();
public:
	void Clean();
	bool OnRead(int32 bytes);
	bool OnWrite(int32 bytes);

public:
	BYTE* ReadPos() { return &buffer[readPos]; }
	BYTE* WritePos() {return &buffer[writePos]; }
	int32 DataSize() { return writePos - readPos; }
	//여유 공간은 총용량에서 write한 위치까지 뺀 나머지
	int32 FreeSize() { return capacity - writePos; }
};

